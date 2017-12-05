
(* ============================================================ *)
(*  THIS IS THE EMPTY VERSION, THAT IS REQUIRED TO BOOTSTRAP    *)
(*  THE JVM VERSION WITHOUT THE MSCORLIB ASSEMBLY AVAILABLE.    *)
(* ============================================================ *)
(* ============================================================ *)

MODULE LlvmUtil;

  IMPORT
        Console, 
        GPCPcopyright,
        GPBinFiles,
        GPText,
        Cst := CompState,
        Id  := IdDesc,
        Lv  := LitValue,
        Sy  := Symbols,
        Ty  := TypeDesc,
        Bi  := Builtin,
        ASCII, RTS;

(* ============================================================ *)

  TYPE LlvmFile*  = POINTER TO RECORD
                      srcS* : LitValue.CharOpen; (* source file name   *)
                      outN* : LitValue.CharOpen; (* output file name   *)
                      proc* : ProcInfo;
                      nxtLb : INTEGER;
                      clsN* : Lv.CharOpen; (* current class name *)
                      file* : GPBinFiles.FILE;
                      work- : Sy.IdSeq;
                    END;

  TYPE UByteArrayPtr = POINTER TO ARRAY OF UBYTE;

  TYPE Label*     = POINTER TO ABSTRACT RECORD END;
       LbArr*     = POINTER TO ARRAY OF Label;

  TYPE ProcInfo*  = POINTER TO (* EXTENSIBLE *) RECORD
                      prId- : Sy.Scope; (* mth., prc. or mod.   *)
                      rtLc* : INTEGER;  (* return value local # *)
                      (* ---- depth tracking ------ *)
                      dNum- : INTEGER;  (* current stack depth  *)
                      dMax- : INTEGER;  (* maximum stack depth  *)
                      (* ---- temp-var manager ---- *)
                      lNum- : INTEGER;         (* prog vars     *)
                      tLst- : Sy.TypeSeq;      (* type list     *)
                      fLst- : Sy.TypeSeq;      (* free list     *)
                      (* ---- end temp manager ---- *)
                      exLb* : Label;    (* exception exit label *)
                    END;

(* ============================================================ *)
(* ============================================================ *)

VAR
    rts,                     (* Base name of runtime system *)
    cAt,                     (* @ *)
    dlr,                     (* $ *)
    pcnt,                    (* % *)
    dot,                     (* . *)
    star,                    (* * *)
    lPar,                    (* ( *)
    rPar,                    (* ) *)
    lBrace,                  (* { *)
    rBrace,                  (* } *)
    lBrac,                   (* [ *)
    rBrac,                   (* ] *)
    semi,                    (* ; *)
    comma,                   (* , *)
    ptrTo,                   (* _* *)
    commaS,                  (* ,_ *)
    times,                   (* _x_ *)
    equals,                  (* _=_ *)
    void,                    (* void *)
    synCls,                  (* .$$. *)
    globSec,                 (* global *)
    extStr,                  (* external *)
    intStr,                  (* internal *)
                             (* llvm integer types ... *)
    i01, i08, i16, i32, i64 : Lv.CharOpen;

(* ============================================================ *)
(*                    Constructor Method                        *)
(* ============================================================ *)
  PROCEDURE^ MkBlkName*(mod : Id.BlkId);
(* ============================================================ *)

  PROCEDURE newLlvmFile*(mod : Id.BlkId) : LlvmFile;
    VAR fil : LlvmFile;
        nam : Lv.CharOpen;
  BEGIN
    NEW(fil);
    Sy.InitIdSeq(fil.work, 4);
   (*
    *  Append module BlkId at head of list.
    *  This represents the synthetic, static class.
    *)
    Sy.AppendIdnt(fil.work, mod);
    MkBlkName(mod);
    nam := mod.pkgNm;
   (*
    *  Create the file.
    *)
    fil.outN := BOX(nam^ + ".ll");
    fil.file := GPBinFiles.createFile(fil.outN);
    RETURN fil;
  END newLlvmFile;

(* ============================================================ *)

  PROCEDURE (t : LlvmFile)fileOk*() : BOOLEAN,NEW;
  BEGIN
    RETURN t.file # NIL;
  END fileOk;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)WorklistAdd*(idnt : Id.TypId),NEW;
  BEGIN 
    Sy.AppendIdnt(os.work, idnt);   
  END WorklistAdd;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)MkNewProcInfo*(proc : Sy.Scope),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END MkNewProcInfo;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)newLabel*() : Label,NEW;
  BEGIN
    RETURN NIL;
  END newLabel;

(* ============================================================ *)
(*               Private Vector Wrapper Methods                 *)
(* ============================================================ *)

  PROCEDURE catenate2(a, b : Lv.CharOpen) : Lv.CharOpen;
  BEGIN
    RETURN BOX(a^ + b^);
  END catenate2;

  PROCEDURE catenate3(a, b, c : Lv.CharOpen) : Lv.CharOpen;
    VAR builder : VECTOR OF Lv.CharOpen;
  BEGIN
    NEW(builder, 4);
    APPEND(builder, a); APPEND(builder, b); APPEND(builder, c);
    RETURN Lv.vectorCat(builder);
  END catenate3;

  PROCEDURE catenate4(a, b, c, d : Lv.CharOpen) : Lv.CharOpen;
    VAR builder : VECTOR OF Lv.CharOpen;
  BEGIN
    NEW(builder, 4);
    APPEND(builder, a); APPEND(builder, b); APPEND(builder, c); APPEND(builder, d);
    RETURN Lv.vectorCat(builder);
  END catenate4;

  PROCEDURE catenate5(a, b, c, d, e : Lv.CharOpen) : Lv.CharOpen;
    VAR builder : VECTOR OF Lv.CharOpen;
  BEGIN
    NEW(builder, 6);
    APPEND(builder, a); APPEND(builder, b); APPEND(builder, c); APPEND(builder, d); APPEND(builder, e);
    RETURN Lv.vectorCat(builder);
  END catenate5;

  PROCEDURE catenateArr(arr : ARRAY OF Lv.CharOpen) : Lv.CharOpen;
    VAR builder : VECTOR OF Lv.CharOpen;
        element : Lv.CharOpen;
        index : INTEGER;
  BEGIN
    NEW(builder, 8);
    FOR index := 0 TO LEN(arr) - 1 DO
      element := arr[index];
      IF (element # NIL) & (LEN(element) > 0) THEN APPEND(builder, element) END;
    END;
    RETURN Lv.vectorCat(builder);
  END catenateArr;

(* ==================================================================== *
 *             A word on naming for the llvm version.                   *
 * ==================================================================== *
 *  Part one: module-level declarations, in Module Mmm.                 *
 *        globals in this module, say,                                  *
 *              VAR var* : INTEGER;                                     *
 *        will have an llvm name                                        *
 *              @Mmm.$$.var = ...                                       *
 *        A record defined at outer level in Mmm, say,                  *
 *              TYPE Ttt* = RECORD ... END;                             *
 *        has llvm struct name                                          *
 *              %Mmm.Ttt = type { ... }                                 *
 *        Note that types always have local names only.                 *
 *        A (static) procedure at outer level in Mmm, say,              *
 *              PROCEDURE Foo*(...) : T; END Foo;                       *
 *        will have an llvm name                                        *
 *              define <retType> @Mmm.Foo(<argtypes>) { ... }           *
 *                                                                      *
 *  Part two: nested declarations, in Module Mmm.                       *
 *        A nested type can occur with source such as
 *              TYPE Ttt* = RECORD a : RECORD x,y : CHAR END END;       *
 *        in this case the nested record is anonymous                   *
 *        In llvm this will have the struct declarations --             *
 *              %Mmm.Ttt = type {%Mmm.Ttt$Tnnn}                         *
 *              %Mmm.Ttt$Tnnn = type {i16,i16}                          *
 *                                                                      *
 *  Part three: nested declarations in Procedure Mmm.Foo                *
 *        Consider                                                      *
 *              PROCEDURE Foo(...) ...                                  *
 *                TYPE Tn = RECORD ... END;                             *
 *                PROCEDURE Nest(...); ... END Nest;                    *
 *              END Foo;                                                *
 *        In llvm this will be defined as --                            *
 *              define <r1> @Mmm.Foo(...) {...}                         *
 *              define <r2> %Mmm.Foo$Nest(...) { ... )                  *
 *              %Mmm.Foo$Tn = type { ... }                              * 
 *                                                                      *
 *  Part four: type-bound procedures bound to Type Ttt in module Mmm    *
 *        Consider                                                      *
 *              PROCEDURE (VAR this : Ttt)Bar*(...) : Rt; END Bar;      *
 *        In llvm this will be defined as --                            *
 *              define <ret> @Mmm.Ttt.Bar(%Mmm.Ttt *, ... ) { ... }     *
 *                                                                      *
 * ==================================================================== *)
(* ==================================================================== *)



(* ============================================================ *)
(*                    Namehandling Methods                      *)
(* ============================================================ *)

 (*
  *  The prefix name that all named objects
  *  in this block will use is mod.xName.
  *)
  PROCEDURE MkBlkName*(mod : Id.BlkId);
    VAR mNm : Lv.CharOpen;
        fil : Lv.CharOpen;
  (* -------------------------------------------------- *)
    PROCEDURE scpMangle(mod : Id.BlkId) : Lv.CharOpen;
      VAR outS : Lv.CharOpen;
    BEGIN
      IF mod.kind = Id.impId THEN
        outS := catenate4(dot,mod.pkgNm,dot,mod.xName);
      ELSE
        outS := mod.xName;
      END;
      IF LEN(mod.xName$) > 0 THEN outS := catenate2(outS, dot) END;
      RETURN outS;
    END scpMangle;
  (* -------------------------------------------------- *)
    PROCEDURE nmSpaceOf(mod : Id.BlkId) : Lv.CharOpen;
      VAR ix : INTEGER;
          ln : INTEGER;
          ch : CHAR;
          inS : Lv.CharOpen;
    BEGIN
      inS := mod.scopeNm;
      IF inS[0] # '[' THEN 
        RETURN inS;
      ELSE
        ln := LEN(inS);
        ix := 0;
        REPEAT
          ch := inS[ix]; 
          INC(ix);
        UNTIL (ix = LEN(inS)) OR (ch = ']');
        RETURN Lv.subChOToChO(inS, ix, ln-ix);
      END;
    END nmSpaceOf;
  (* -------------------------------------------------- *)
    PROCEDURE pkgNameOf(mod : Id.BlkId) : Lv.CharOpen;
      VAR ix : INTEGER;
          ln : INTEGER;
          ch : CHAR;
          inS : Lv.CharOpen;
    BEGIN
      inS := mod.scopeNm;
      IF inS[0] # '[' THEN 
        RETURN mod.clsNm;
      ELSE
        INCL(mod.xAttr, Sy.isFn); (* make sure this is marked foreign *)
        ln := LEN(inS);
        ix := 0;
        REPEAT
          ch := inS[ix]; 
          INC(ix);
        UNTIL (ix = LEN(inS)) OR (ch = ']');
        RETURN Lv.subChOToChO(inS, 1, ix-2);
      END;
    END pkgNameOf;
  (* -------------------------------------------------- *)
  BEGIN
    IF mod.xName # NIL THEN RETURN END;
    mNm := Sy.getName.ChPtr(mod);    
    IF mod.scopeNm # NIL THEN 
    (* FIXME *)
      IF mod.clsNm = NIL THEN
        mod.clsNm   := mNm;            (* dummy class name  *)
      END;
      mod.pkgNm   := pkgNameOf(mod);   (* assembly filename *)
      mod.xName   := nmSpaceOf(mod);   (* namespace name    *)
      mod.scopeNm := scpMangle(mod);   (* class prefix name *)
    ELSE (* mainline for CP modules *)
      mod.pkgNm   := mNm;              (* output basename   *)
      mod.xName   := mNm;              (* namespace name    *)
     (*
      *  In the normal case, the assembly name is the
      *  same as the module name.  However, system 
      *  modules always have the assembly name "CPLLRTS".
      *)
      IF Sy.rtsMd IN mod.xAttr THEN 
        mod.clsNm   := catenate3(rts, dot, mNm);
        mod.scopeNm := mod.clsNm;
      ELSE
        mod.clsNm   := catenate2(mNm,synCls);
        mod.scopeNm := mNm;            (* class prefix name *)
      END;
    END;
  END MkBlkName;

(* ============================================================ *)
  PROCEDURE^ MkProcName*(proc : Id.Procs; os : LlvmFile);
  PROCEDURE^ MkAliasName*(typ : Ty.Opaque; os : LlvmFile);
  PROCEDURE^ MkIdName*(id : Sy.Idnt; os : LlvmFile);
  PROCEDURE^ MkRecName*(typ : Ty.Record; os : LlvmFile);
  PROCEDURE^ MkPtrName*(typ : Ty.Pointer; os : LlvmFile);
  PROCEDURE^ MkPTypeName*(typ : Ty.Procedure; os : LlvmFile);
  PROCEDURE^ MkArrName(typ : Ty.Array; os : LlvmFile);
  PROCEDURE^ MkVecName(typ : Ty.Vector; os : LlvmFile);
  PROCEDURE^ typeName(typ : Sy.Type; os : LlvmFile) : Lv.CharOpen;


(* ============================================================ *)

  PROCEDURE MkTypeName*(typ : Sy.Type; fil : LlvmFile);
  BEGIN
    WITH typ : Ty.Vector  DO MkVecName(typ, fil);
      |  typ : Ty.Array   DO MkArrName(typ, fil);
      |  typ : Ty.Record  DO MkRecName(typ, fil);
      |  typ : Ty.Pointer DO MkPtrName(typ, fil);
      |  typ : Ty.Opaque  DO MkAliasName(typ, fil);
      |  typ : Ty.Procedure DO MkPTypeName(typ, fil);
    END;
  END MkTypeName;

(* ============================================================ *)

  PROCEDURE MkRecName*(typ : Ty.Record; os : LlvmFile);
    VAR mNm : Lv.CharOpen;      (* prefix scope name    *)
        rNm : Lv.CharOpen;      (* simple name of type  *)
        tId : Sy.Idnt;
        scp : Sy.Scope;
  (* ---------------------------------------------------------- *)
  BEGIN
   (*
    *  For Record types the fully qualified 
    *  name is constructed in typ.scopeNm
    *
    *  First, ensure that the super type has been processed.
    *)
    IF (typ.baseTp IS Ty.Record) &
       (typ.baseTp.xName = NIL) THEN MkRecName(typ.baseTp(Ty.Record), os) END;
   (*
    *  Now, get the simple name of the record type.
    *)
    IF typ.bindTp # NIL THEN              (* Synthetically named rec'd *)
      tId := typ.bindTp.idnt;
      rNm := Sy.getName.ChPtr(tId);
    ELSE                                  (* Normal, named record type *)
      IF typ.idnt = NIL THEN              (* Anonymous record type     *)
        typ.idnt := Id.newAnonId(typ.serial);
        typ.idnt.type := typ;
      END;
      tId := typ.idnt;
      rNm := Sy.getName.ChPtr(tId);
    END;
    typ.xName := rNm;

    IF tId.dfScp = NIL THEN tId.dfScp := Cst.thisMod END;
    scp := tId.dfScp;
   (*
    *  At this program point the situation is as follows:
    *  rNm holds the simple name of the record. The scope
    *  in which the record is defined is scp.
    *)
    WITH scp : Id.Procs DO
        (* FIXME *)
        IF scp.prcNm = NIL THEN MkProcName(scp, os) END;
        typ.scopeNm := catenate4(pcnt, scp.prcNm, dlr, rNm);
    | scp : Id.BlkId DO
        IF scp.xName = NIL THEN MkBlkName(scp) END;
        typ.scopeNm := catenate4(pcnt, scp.scopeNm, dot, rNm);
    END;
(* FIXME : Same code for PTypes? *)
    IF typ.tgXtn # os THEN (* This Rec not on worklist *)
      os.WorklistAdd(tId(Id.TypId));
      typ.tgXtn := os;
    END;
  END MkRecName;

(* ============================================================ *)

  PROCEDURE MkPtrName*(typ : Ty.Pointer; os : LlvmFile);
  BEGIN
   (*
    *  For Pointer types the fully qualified name typ.xName
    *)
    typ.xName := BOX(typeName(typ.boundTp, os)^ + ptrTo^);
  END MkPtrName;

(* ============================================================ *)

  PROCEDURE MkPTypeName*(typ : Ty.Procedure; os : LlvmFile);
    VAR mNm : Lv.CharOpen;      (* prefix scope name    *)
        tNm : Lv.CharOpen;
        sNm : Lv.CharOpen;
  BEGIN
   (*
    *  For Procedure types the fully qualified 
    *  name is constructed in typ.tName
    *)
    IF typ.xName # NIL THEN RETURN END;
   (*
    *  Set the eName field
    *)
    IF typ.idnt = NIL THEN              (* Anonymous procedure type *)
      typ.idnt := Id.newAnonId(typ.serial);
      typ.idnt.type := typ;
    END;
    IF typ.idnt.dfScp = NIL THEN typ.idnt.dfScp := Cst.thisMod END;
    MkIdName(typ.idnt.dfScp, os); 

    sNm := typ.idnt.dfScp.scopeNm;
    tNm := Sy.getName.ChPtr(typ.idnt);
    typ.tName := catenate4(pcnt, sNm, dot, tNm);
    typ.xName := tNm;
(* FIXME *)
  END MkPTypeName;

(* ============================================================ *)

  PROCEDURE MkArrName*(typ : Ty.Array; os : LlvmFile);
  BEGIN
   (*
    *  For Array types the fully qualified name is typ.xName
    *)
    typ.xName := catenate5(lBrac, Lv.intToCharOpen(typ.length), 
                              times, typeName(typ.elemTp, os), rBrac);
  END MkArrName;

(* ============================================================ *)

  PROCEDURE MkVecName*(typ : Ty.Vector; os : LlvmFile);
  BEGIN RTS.Throw("Not implemented exception");
   (*
    *  For Vector types the fully qualified name is typ.xName
    *)
  END MkVecName;

(* ============================================================ *)

  PROCEDURE MkAliasName*(typ : Ty.Opaque; os : LlvmFile);
  BEGIN RTS.Throw("Not implemented exception");
   (*
    *  For Alias types the fully qualified name is typ.xName
    *)
  END MkAliasName;

(* ============================================================ *)
(* ============================================================ *)

  PROCEDURE MkProcName*(proc : Id.Procs; os : LlvmFile);
  BEGIN RTS.Throw("Not implemented exception");
  END MkProcName;

(* ============================================================ *)

  PROCEDURE MkVarName*(var : Id.VarId; os : LlvmFile);
  BEGIN
    IF var.recTyp = NIL THEN     (* normal case  *)
      var.clsNm := var.dfScp(Id.BlkId).clsNm;
    ELSE                         (* static field *)
      IF var.recTyp.xName = NIL THEN MkTypeName(var.recTyp, os) END;
      var.clsNm := var.recTyp.xName; 
    END;
    var.varNm := catenate3(cAt, var.clsNm, Sy.getName.ChPtr(var));
  END MkVarName;

(* ============================================================ *)

  PROCEDURE MkFldName*(var : Id.FldId; os : LlvmFile);
  BEGIN RTS.Throw("Not implemented exception");
  END MkFldName;

(* ============================================================ *)

  PROCEDURE MkIdName*(id : Sy.Idnt; os : LlvmFile);
  BEGIN
    WITH id : Id.Procs DO IF id.scopeNm = NIL THEN MkProcName(id, os) END;
    |    id : Id.BlkId DO IF id.scopeNm = NIL THEN MkBlkName(id)      END;
    |    id : Id.VarId DO IF id.varNm   = NIL THEN MkVarName(id, os)  END;
    |    id : Id.FldId DO IF id.fldNm   = NIL THEN MkFldName(id, os)  END;
    |    id : Id.LocId DO (* skip *)
    END;
  END MkIdName;
 


(* ============================================================ *)

  

(* ============================================================ *)
(*                    Private Methods                           *)
(* ============================================================ *)

(*  PROCEDURE^ (os : LlvmFile)Comment*(IN s : ARRAY OF CHAR),NEW;  *)

(* ============================================================ *)

  PROCEDURE varName(var : Id.VarId; os : LlvmFile) : Lv.CharOpen;
    VAR exported : BOOLEAN;
  BEGIN
    IF var.varNm = NIL THEN MkVarName(var, os) END;
    RETURN var.varNm;
  END varName;

(* ============================================================ *)

  PROCEDURE typeName(typ : Sy.Type; os : LlvmFile) : Lv.CharOpen;
  BEGIN
   (* 
    *  For every kind of name the invariant holds---
    *  xName = NIL ==> name is not yet built.
    *)
    IF typ = NIL THEN RETURN void END;
    IF typ.xName = NIL THEN MkTypeName(typ, os) END;
    WITH typ : Ty.Base DO
        RETURN typ.xName;
    | typ : Ty.Array DO
        RETURN typ.xName;
    | typ : Ty.Record DO
        RETURN typ.scopeNm;
    | typ : Ty.Pointer DO
        RETURN typ.xName;
    | typ : Ty.Opaque DO
        RETURN typ.xName;
    | typ : Ty.Enum DO
        RETURN Bi.intTp.xName;
    | typ : Ty.Procedure DO
        RETURN typ.tName;
    END;
  END typeName;

(* ============================================================ *)

 (*
  * Coerce char array to byte array. 
  * Only valid if chars are 8-bit only.
  *)
  PROCEDURE UBytesOf(IN str : ARRAY OF CHAR) : UByteArrayPtr;
    VAR result : UByteArrayPtr;
        index  : INTEGER;
        length : INTEGER;
  BEGIN
    length := LEN(str$);
    NEW(result, length);
    FOR index := 0 TO length-1 DO
      result[index] := USHORT(ORD(str[index]) MOD 256);
    END;
    RETURN result;
  END UBytesOf;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)CatChar(chr : CHAR),NEW;
  BEGIN
    GPBinFiles.WriteByte(os.file, ORD(chr) MOD 256);
  END CatChar;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)CatStr(IN str : ARRAY OF CHAR),NEW;
  BEGIN
    GPBinFiles.WriteNBytes(os.file, UBytesOf(str), LEN(str$));
  END CatStr;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)CatInt(val : INTEGER),NEW;
    VAR arr : ARRAY 16 OF CHAR;
  BEGIN
    GPText.IntToStr(val, arr); os.CatStr(arr);
  END CatInt;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)CatLong(val : LONGINT),NEW;
    VAR arr : ARRAY 32 OF CHAR;
  BEGIN
    GPText.LongToStr(val, arr); os.CatStr(arr);
  END CatLong;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)CatEOL(),NEW;
  BEGIN
    os.CatStr(RTS.eol);
  END CatEOL;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)WriteHex(int : INTEGER),NEW;
    VAR ord : INTEGER;
  BEGIN
    IF int <= 9 THEN ord := ORD('0') + int ELSE ord := (ORD('A')-10)+int END;
    os.CatChar(CHR(ord));
  END WriteHex;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)WriteHexByte(int : INTEGER),NEW;
  BEGIN
    os.WriteHex(int DIV 16);
    os.WriteHex(int MOD 16);
    os.CatChar(' ');
  END WriteHexByte;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Tstring(IN str : ARRAY OF CHAR),NEW;
   (* TAB, then string *)
  BEGIN
    os.CatChar(ASCII.HT); os.CatStr(str);
  END Tstring;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Bstring(IN str : ARRAY OF CHAR),NEW;
   (* BLANK, then string *)
  BEGIN
    os.CatChar(" "); os.CatStr(str);
  END Bstring;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Tint(int : INTEGER),NEW;
   (* TAB, then int *)
  BEGIN
    os.CatChar(ASCII.HT); os.CatInt(int);
  END Tint;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Tlong(long : LONGINT),NEW;
   (* TAB, then long *)
  BEGIN
    os.CatChar(ASCII.HT); os.CatLong(long);
  END Tlong;

(* ============================================================ *)
  
  PROCEDURE (os : LlvmFile)QuoteStr(IN str : ARRAY OF CHAR),NEW;
   (* ------------------------ *)
    PROCEDURE EmitQuotedString(os : LlvmFile; IN str : ARRAY OF CHAR);
      VAR chr : CHAR;
          idx : INTEGER;
          ord : INTEGER;
    BEGIN
      os.CatChar('"');
      FOR idx := 0 TO LEN(str) - 2 DO
        chr := str[idx];
        CASE chr OF
        | "\",'"' : os.CatChar("\");
                    os.CatChar(chr);
        | 9X      : os.CatChar("\");
                    os.CatChar("t");
        | 0AX     : os.CatChar("\");
                    os.CatChar("n");
        ELSE
          IF chr > 07EX THEN
            ord := ORD(chr);
            os.CatChar('\');
            os.CatChar(CHR(ord DIV 64 + ORD('0')));
            os.CatChar(CHR(ord MOD 64 DIV 8 + ORD('0')));
            os.CatChar(CHR(ord MOD 8 + ORD('0')));
          ELSE
            os.CatChar(chr);
          END
        END;
      END;
      os.CatChar('"');
    END EmitQuotedString;
   (* ------------------------ *)
    PROCEDURE EmitByteArray(os : LlvmFile; IN str : ARRAY OF CHAR);
      VAR idx : INTEGER;
          ord : INTEGER;
    BEGIN
      os.CatStr("bytearray (");
      FOR idx := 0 TO LEN(str) - 2 DO
        ord := ORD(str[idx]);
        os.WriteHexByte(ord MOD 256);
        os.WriteHexByte(ord DIV 256);
      END;
      os.CatStr(rPar);
    END EmitByteArray;
   (* ------------------------ *)
    PROCEDURE NotASCIIZ(IN str : ARRAY OF CHAR) : BOOLEAN;
      VAR idx : INTEGER;
          ord : INTEGER;
    BEGIN
      FOR idx := 0 TO LEN(str) - 2 DO
        ord := ORD(str[idx]);
        IF (ord = 0) OR (ord > 0FFH) THEN RETURN TRUE END;
      END;
      RETURN FALSE;
    END NotASCIIZ;
   (* ------------------------ *)
  BEGIN
    IF NotASCIIZ(str) THEN
      EmitByteArray(os, str);
    ELSE
      EmitQuotedString(os, str);
    END;
  END QuoteStr;

(* ============================================================ *)
(*                    Exported Methods                  *)
(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Comment*(IN s : ARRAY OF CHAR),NEW;
  BEGIN
    os.CatStr("; ");
    os.CatStr(s);
    os.CatEOL();
  END Comment;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Blank*(),NEW;
  BEGIN
    os.CatEOL();
  END Blank;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)EmitStaticField*(id : Id.VarId),NEW;
  BEGIN
    (* Emit an LLVM global or local variable *)
    os.CatStr(varName(id, os));
    os.CatStr(equals);
    IF id.vMod = Sy.prvMode THEN os.CatStr(intStr) END; (* internal *)
    os.CatStr(globSec);
    os.CatStr(typeName(id.type, os));
    os.CatEOL();
  END EmitStaticField;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)EmitOneStruct*(typ : Ty.Record),NEW;
    VAR index : INTEGER;
        tName : Lv.CharOpen;
   (* ------------------------------- *)
    PROCEDURE EmitFieldList(os : LlvmFile; rec : Ty.Record; VAR ix : INTEGER);
      VAR baseRec : Ty.Record;
          index : INTEGER;
    BEGIN
      IF rec = NIL THEN RETURN END;
      IF rec.baseTp # NIL THEN EmitFieldList(os, Ty.baseRecTp(rec), ix) END;
      FOR index := 0 TO rec.fields.tide - 1 DO
        IF ix # 0 THEN os.CatStr(commaS) END;
        os.CatStr(typeName(rec.fields.a[index].type, os)); INC(ix);
      END;
    END EmitFieldList;
   (* ------------------------------- *)
  BEGIN
    (* Emit an LLVM global or local variable *)
    tName := typeName(typ, os);
    os.Comment("Struct for RECORD type " + typ.xName^);
    os.CatStr(tName);
    os.CatStr(" = type {");
    index := 0;
    EmitFieldList(os, typ, index);
    os.CatStr(rBrace);
    os.CatEOL();
  END EmitOneStruct;
(* ============================================================ *)

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)EmitPTypeDecl*(tId : Id.TypId),NEW;
    VAR proc : Ty.Procedure;
        name : Lv.CharOpen;
   (* ------------------------------- *)
    PROCEDURE EmitArgList(os : LlvmFile; prc : Ty.Procedure);
      VAR index : INTEGER;
          param : Id.ParId;
    BEGIN
      IF prc.receiver # NIL THEN
        os.CatStr(typeName(prc.receiver, os));
        IF prc.receiver.isRecordType() THEN os.CatStr(ptrTo) END;
      END;
      FOR index := 0 TO prc.formals.tide - 1 DO
        param := prc.formals.a[index];
        IF (index # 0) OR (prc.receiver # NIL) THEN os.CatStr(commaS) END;
        os.CatStr(typeName(param.type, os));
        IF param.parMod # Sy.val THEN os.CatStr(ptrTo) END;
      END;
    END EmitArgList;
   (* ------------------------------- *)
  BEGIN
    proc := tId.type(Ty.Procedure);
    name := typeName(proc, os);
    os.Comment("Typedef for PROCEDURE type " + proc.xName^);
    os.CatStr(name);
    os.CatStr(" = type ");
    os.CatStr(typeName(proc.retType, os));
    os.CatStr(" (");
    EmitArgList(os, proc);
    os.CatStr(") *"); 
    os.CatEOL();
  END EmitPTypeDecl;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)EmitStructField*(id : Id.FldId),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END EmitStructField;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Separator(c : CHAR; i : INTEGER),NEW;
  BEGIN
    os.CatChar(c);
    os.CatEOL();
    WHILE i > 0 DO os.CatChar(ASCII.HT); DEC(i) END;
  END Separator;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)OpenBrace*(i : INTEGER),NEW;
  BEGIN
    WHILE i > 0 DO os.CatChar(" "); DEC(i) END;
    os.CatChar("{");
    os.CatEOL();
  END OpenBrace;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)CloseBrace*(i : INTEGER),NEW;
  BEGIN
    WHILE i > 0 DO os.CatChar(" "); DEC(i) END;
    os.CatChar("}");
    os.CatEOL();
  END CloseBrace;

(* ============================================================ *)

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)MethodDecl*(attr : SET; proc : Id.Procs),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END MethodDecl;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)ExternList*(),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END ExternList;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)DefLab*(l : Label),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END DefLab;

(* -------------------------------------------- *)

  PROCEDURE (os : LlvmFile)DefLabC*(l : Label; IN c : ARRAY OF CHAR),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END DefLabC;

(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Code*(code : INTEGER),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END Code;

(* -------------------------------------------- *)

  PROCEDURE (os : LlvmFile)CodeI*(code,int : INTEGER),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END CodeI;

(* -------------------------------------------- *)

  PROCEDURE (os : LlvmFile)CodeT*(code : INTEGER; type : Sy.Type),NEW;
  BEGIN RTS.Throw("Not implemented exception");
  END CodeT;

(* -------------------------------------------- *)

  PROCEDURE (os : LlvmFile)Header*(IN str : ARRAY OF CHAR),NEW;
    VAR date : ARRAY 64 OF CHAR;
  BEGIN
   (*
    os.srcS := Lv.strToCharOpen(
          "'" + EliminatePathFromSrcName(Lv.strToCharOpen(str))^ + "'");
    *)
    RTS.GetDateString(date);
    os.Comment("LLVM output from GPCP v"+
                GPCPcopyright.V()^ + 
				" (" + RTS.defaultTarget + " host)");
    os.Comment("at date: " + date);
    os.Comment("from source file <" + str + '>');
  END Header;



(* ============================================================ *)

  PROCEDURE (os : LlvmFile)Finish*(),NEW;
  BEGIN 
    GPBinFiles.CloseFile(os.file);
  END Finish;

(* ============================================================ *)
(* ============================================================ *)


(* ============================================================ *)
(* ============================================================ *)
BEGIN
    rts  := BOX("CPLLRTS");
    cAt  := BOX("@");
    dlr  := BOX("$");
    dot  := BOX(".");
    pcnt := BOX("%");
    star := BOX("*");
    i01  := BOX("i1");
    i08  := BOX("i8");
    i16  := BOX("i16");
    i32  := BOX("i32");
    i64  := BOX("i64");
    lPar := BOX("(");
    rPar := BOX(")");
    semi := BOX(";");
    void := BOX("void");
    comma := BOX(",");
    lBrac := BOX("[");
    rBrac := BOX("]");
    times := BOX(" x ");
    ptrTo := BOX(" *");
    lBrace := BOX("{");
    rBrace := BOX("}");
    commaS := BOX(", ");
    equals := BOX(" = ");
    synCls := BOX(".$$.");
    extStr := BOX("external ");
    intStr := BOX("internal ");
    globSec := BOX("global ");

  Bi.setTp.xName  := i32;
  Bi.boolTp.xName := i01;
  Bi.byteTp.xName := i08;
  Bi.uBytTp.xName := i08;
  Bi.charTp.xName := i16;
  Bi.sChrTp.xName := i08;
  Bi.sIntTp.xName := i16;
  Bi.lIntTp.xName := i64;
  Bi.realTp.xName := Lv.strToCharOpen("double");
  Bi.sReaTp.xName := Lv.strToCharOpen("float");
  Bi.intTp.xName  := i32;
  Bi.anyRec.xName := Lv.strToCharOpen("opaque"); (* CHECK and FIXME *)
  Bi.anyPtr.xName := Bi.anyRec.xName;
END LlvmUtil.
(* ============================================================ *)
(* ============================================================ *)

