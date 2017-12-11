(* ============================================================ *)
(*  LlvmMaker is the concrete class for emitting llvm BitCodes  *)
(*  John Gough November 2012     .                              *)
(* ============================================================ *)

MODULE LlvmMaker;

  IMPORT 
        GPCPcopyright,
        ASCII,
        Error,
        Console,
        CPascalS,
        FileNames,
        ClassMaker,
        GPFiles,
        Lv := LitValue,
        Lu := LlvmUtil,
        Cst := CompState,
        Bi := Builtin,
        Sy := Symbols,
        Id := IdDesc,
        Ty := TypeDesc,
        Xp := ExprDesc,
        St := StatDesc;

(* ------------------------------------ *)

  TYPE BitCodeEmitter* =
        POINTER TO
          RECORD (ClassMaker.ClassEmitter)
            (* mod* : Id.BlkId; *)   
            outF : Lu.LlvmFile;
          END;

(* ------------------------------------ *)

  TYPE BitCodeAssembler* = POINTER TO
          RECORD (ClassMaker.Assembler) emit : Lu.LlvmFile END;

(* ------------------------------------ *)

  TYPE BlkIdSFA* = POINTER TO RECORD (Sy.SymForAll) llFile* : Lu.LlvmFile END;

(* ------------------------------------ *)

  VAR  asmName   : Lv.CharOpen;
       asmExe    : BOOLEAN;

(* ============================================================ *)
(* ========          BlkIdSFA visitor method            ======= *)
(* ============================================================ *)

  PROCEDURE (t : BlkIdSFA)Op*(id : Sy.Idnt);
    VAR tpId : Id.TypId;
        type : Sy.Type;
   (* ----------------------------------- *)
    PROCEDURE GetRecOrProc(ty : Sy.Type) : Sy.Type;
      VAR next : Sy.Type;
	BEGIN
	  LOOP
        WITH ty : Ty.Pointer DO
          next := ty.boundTp;
        | ty : Ty.Array DO
		  next := ty.elemTp;
        END;
		IF (next.kind = Ty.ptrTp) OR (next.kind = Ty.arrTp) THEN
          ty := next;
        ELSE
          RETURN next;
        END;
      END;
    END GetRecOrProc;
   (* ----------------------------------- *)
  BEGIN
    IF id.kind = Id.typId THEN
      tpId := id(Id.TypId);
      type := tpId.type;
     (* 
      * If this is a pointer to record, get the bound type.
      *)
	  IF (type.kind = Ty.ptrTp) OR (type.kind = Ty.arrTp) THEN 
        type := GetRecOrProc(type); 
      END;
     (*
	  *  If this is not on the list then add to list
      *)
      IF (type.kind = Ty.recTp) OR (type.kind = Ty.prcTp) THEN
	(*  IF (type.tgXtn = NIL) OR ~(type.tgXtn IS Lu.LlvmFile) THEN *)
	    IF (type.tgXtn # t.llFile) THEN
          t.llFile.WorklistAdd(tpId);
          type.tgXtn := t.llFile; (* Mark as on list *)
		END;
      END;
    END;
  END Op;

(* ============================================================ *)
(* ============================================================ *)

  PROCEDURE newBitCodeEmitter*(mod : Id.BlkId) : BitCodeEmitter;
    VAR emitter : BitCodeEmitter;
  BEGIN
    NEW(emitter);
    emitter.mod := mod;
    RETURN emitter;
  END newBitCodeEmitter;

(* ------------------------------------- *)

  PROCEDURE (this : BitCodeEmitter)InitializeWorklist(),NEW;
    VAR allRecsAndProcTypes : BlkIdSFA;
  BEGIN
   (*
    *  Create symbol table visitor and link
    *  all record typIds to the worklist.
    *)
    NEW(allRecsAndProcTypes);
    allRecsAndProcTypes.llFile := this.outF;
    this.mod.symTb.Apply(allRecsAndProcTypes);
  END InitializeWorklist;

(* ============================================================ *)

  PROCEDURE newBitCodeAssembler*() : BitCodeAssembler;
    VAR asm : BitCodeAssembler;
  BEGIN
    NEW(asm);
    (* FIXME ... Init(); *)
    RETURN asm;
  END newBitCodeAssembler;

(* ============================================================ *)
(* 
  PROCEDURE fieldAttr(id : Sy.Idnt; in : SET) : SET;
  BEGIN
    IF id.type IS Ty.Event THEN (* backing field of event *)
      RETURN in + Asm.att_private;   
    ELSIF id.vMod # Sy.prvMode THEN
      RETURN in + Asm.att_public;
    ELSE
      RETURN in + Asm.att_assembly;
    END;
  END fieldAttr;
 *)
(* ============================================================ *)
(*  Creates basic imports for java.lang, and inserts a few type *)
(*  descriptors for Object, Exception, and String.              *)
(* ============================================================ *)

  PROCEDURE (this : BitCodeEmitter)Init*();
    VAR tId : Id.TypId;
        blk : Id.BlkId;
        obj : Id.TypId;
        cls : Id.TypId;
        str : Id.TypId;
        exc : Id.TypId;
        xhr : Id.TypId;
  BEGIN
   (*
    *  Create import descriptor for gpcprts
    *)
    Bi.MkDummyImport("gpcprts", "gpcprts", blk);
    Cst.SetSysLib(blk);
   (*
    *  Create various classes.
    *)
    Bi.MkDummyClass("Object", blk, Ty.isAbs, obj);
    Cst.ntvObj := obj.type;
    Bi.MkDummyClass("String", blk, Ty.noAtt, str);
    Cst.ntvStr := str.type;
    Bi.MkDummyClass("Exception", blk, Ty.extns, exc);
    Cst.ntvExc := exc.type;
    Bi.MkDummyClass("Class", blk, Ty.noAtt, cls);
    Cst.ntvTyp := cls.type;
   (*
    *  Create import descriptor for CP.RTS
    *)
    Bi.MkDummyImport("RTS", "", blk);
    Bi.MkDummyAlias("NativeType", blk, cls.type, Cst.clsId);
    Bi.MkDummyAlias("NativeObject", blk, obj.type, Cst.objId);
    Bi.MkDummyAlias("NativeString", blk, str.type, Cst.strId);
    Bi.MkDummyAlias("NativeException", blk, exc.type, Cst.excId);

    Bi.MkDummyVar("dblPosInfinity",blk,Bi.realTp,Cst.dblInf);
    Bi.MkDummyVar("dblNegInfinity",blk,Bi.realTp,Cst.dblNInf);
    Bi.MkDummyVar("fltPosInfinity",blk,Bi.sReaTp,Cst.fltInf);
    Bi.MkDummyVar("fltNegInfinity",blk,Bi.sReaTp,Cst.fltNInf);
    INCL(blk.xAttr, Sy.need);
   (*
    *  Uplevel addressing stuff.
    *
    Bi.MkDummyImport("$CPJrts$", "CP.CPJrts", blk);
    Bi.MkDummyClass("XHR", blk, Ty.isAbs, xhr);
    Cst.rtsXHR := xhr.type;
    Cst.xhrId.recTyp := Cst.rtsXHR;
    Cst.xhrId.type   := Cst.rtsXHR;
    *)
  END Init;

(* ============================================================ *)

  PROCEDURE (this : BitCodeEmitter)ObjectFeatures*();
    VAR prcSig : Ty.Procedure; 
        thePar : Id.ParId;
  BEGIN
  (*
   *)
  END ObjectFeatures;

(* ============================================================ *)

  PROCEDURE (this : BitCodeEmitter)EmitModStatics(out : Lu.LlvmFile; 
                                                  mod : Id.BlkId),NEW;
  (** Create the assembler for a class file for this module. *)
    VAR index : INTEGER;
        proc  : Id.Procs;
        recT  : Sy.Type;
        varId : Sy.Idnt;
        cfLive : BOOLEAN; (* Control Flow is (still) live *)
        threadDummy : Sy.Stmt;
        threadField : Sy.Idnt;
  BEGIN
   (*
    *  Emit all of the static fields
    *)
    FOR index := 0 TO this.mod.locals.tide-1 DO
      varId := this.mod.locals.a[index];
      out.EmitStaticField(varId(Id.VarId));
    END;
  END EmitModStatics;

(* ============================================================ *)

  PROCEDURE (this : BitCodeEmitter)EmitModBody(out : Lu.LlvmFile; 
                                               mod : Id.BlkId),NEW;
  (** Create the assembler for a class file for this module. *)
    VAR index : INTEGER;
        proc  : Id.Procs;
        recT  : Sy.Type;
        varId : Sy.Idnt;
        cfLive : BOOLEAN; (* Control Flow is (still) live *)
        threadDummy : Sy.Stmt;
        threadField : Sy.Idnt;
  BEGIN
  (* ############################# *
    out.MkBodyClass(mod);

(*
    threadDummy := NIL; (* to avoid warning *)
    IF Sy.sta IN this.mod.xAttr THEN
      this.AddStaMembers();
      threadDummy := this.mkThreadAssign(); 
    END;
 *

    out.OpenBrace(2);
    FOR index := 0 TO this.mod.procs.tide-1 DO
     (*
      *  Create the mangled name for all procedures 
      *  (including static and type-bound methods).
      *)
      proc := this.mod.procs.a[index];
      Mu.MkProcName(proc, out);
      Mu.RenumberLocals(proc, out);
    END;
 *)
   (*
    *  Emit all of the static fields
    *)
    FOR index := 0 TO this.mod.locals.tide-1 DO
      varId := this.mod.locals.a[index];
      out.EmitStaticField(varId(Id.VarId));
    END;
(*
   (*
    *  Emit all of the static event methods
    *)
    FOR index := 0 TO this.mod.locals.tide-1 DO
      varId := this.mod.locals.a[index];
      IF varId.type IS Ty.Event THEN out.EmitEventMethods(varId(Id.AbVar)) END;
    END;
   (*
    *  No constructor for the module "class",
    *  there are never any instances created.
    *)
    asmExe := this.mod.main;    (* Boolean flag for assembler *)
    IF asmExe THEN
     (*
      *   Emit '<clinit>' with variable initialization
      *)
      out.Blank();
      out.MkNewProcInfo(this.mod);
      out.ClinitHead();
      out.InitVars(this.mod);
      out.Code(Asm.opc_ret);
      out.ClinitTail();
      out.Blank();
     (*
      *   Emit module body as 'CPmain() or WinMain'
      *)
      out.MkNewProcInfo(this.mod);
      out.MainHead(this.mod.xAttr);
      IF Sy.sta IN this.mod.xAttr THEN
        out.Comment("Real entry point for STA");
        this.EmitStat(threadDummy, cfLive);
      ELSE
        this.EmitStat(this.mod.modBody, cfLive);
      END;
      IF cfLive THEN
        out.Comment("Continuing directly to CLOSE");
        this.EmitStat(this.mod.modClose, cfLive);
        (* Sequence point for the implicit RETURN *)
        out.LineSpan(Scn.mkSpanT(this.mod.endTok));       
        IF cfLive THEN out.Code(Asm.opc_ret) END;
      END;
      out.MainTail();
    ELSE
     (*
      *   Emit single <clinit> incorporating module body
      *)
      out.MkNewProcInfo(this.mod);
      out.ClinitHead();
      out.InitVars(this.mod);
      this.EmitStat(this.mod.modBody, cfLive);
      IF cfLive THEN out.Code(Asm.opc_ret) END;
      out.ClinitTail();
    END;
   (*
    *  Emit all of the static procedures
    *)
    out.Blank();
    FOR index := 0 TO this.mod.procs.tide-1 DO
      proc := this.mod.procs.a[index];
      IF (proc.kind = Id.conPrc) &
         (proc.dfScp.kind = Id.modId) THEN this.EmitProc(proc, staticAtt) END;
    END;
   (*
    *  And now, just in case exported types that
    *  have class representation have been missed ...
    *)
    FOR index := 0 TO this.mod.expRecs.tide-1 DO
      recT := this.mod.expRecs.a[index];
      IF recT.xName = NIL THEN Mu.MkTypeName(recT, out) END;
    END;

    out.CloseBrace(2);
    out.ClassTail();
*)
 * ############################# *)
  END EmitModBody;

(* ============================================================ *)
(* ============================================================ *)

  PROCEDURE (this : BitCodeEmitter)
                         EmitRecordStruct(out : Lu.LlvmFile; typId : Id.TypId),NEW;
    VAR record : Ty.Record;
  BEGIN
    out.Blank();
    record := typId.type.boundRecTp()(Ty.Record);
   (*
    *  Now emit the struct definition.
    *)
    out.EmitOneStruct(record);
  END EmitRecordStruct;

(* ============================================================ *)
(* ============================================================ *)

  PROCEDURE (this : BitCodeEmitter)MakeAbsName(),NEW;
    VAR nPtr : POINTER TO ARRAY OF CHAR;
        dPtr : POINTER TO ARRAY OF CHAR;
  BEGIN
    IF this.mod.main THEN 
      nPtr := BOX(this.mod.pkgNm$ + ".EXE");
    ELSE
      nPtr := BOX(this.mod.pkgNm$ + ".DLL");
    END;
    IF Cst.binDir # "" THEN
      dPtr := BOX(Cst.binDir$); 
      IF dPtr[LEN(dPtr) - 2] = GPFiles.fileSep THEN
        nPtr := BOX(dPtr^ + nPtr^);
      ELSE
        nPtr := BOX(dPtr^ + "\" + nPtr^);
      END;
    END;
    Cst.outNam := nPtr;
  END MakeAbsName;

(* ============================================================ *)

 PROCEDURE (this : BitCodeEmitter)Emit*();
  (** Create the file-state structure for this output
      module: overrides EMPTY method in ClassMaker  *)
    VAR out       : Lu.LlvmFile; 
        classIx   : INTEGER;
        idDesc    : Sy.Idnt;
        impElem   : Id.BlkId;
        callApi   : BOOLEAN;
  BEGIN
   (*
    *  Emit the code for this module.
    *  First some housekeeping and file operations.
    *)
    callApi := Cst.doCode;
    out := LlvmUtil.newLlvmFile(this.mod);
    this.outF := out;
    IF ~out.fileOk() THEN
      CPascalS.SemError.Report(177, 0, 0);
      Error.WriteString("Cannot create out-file <" + out.outN^ + ">");
      Error.WriteLn;
      RETURN;
    END;
    IF Cst.verbose THEN Cst.Message("Created "+ out.outN^) END;
   (*
    *  Output header information to the output file.
    *)
    out.Blank();
    out.Header(Cst.srcNam);
    IF this.mod.main THEN out.Comment("This module implements CPmain") END;
    out.Blank();
    out.Comment("Non-default target triple would go here");
    out.Blank();
   (*
    *  Load up the worklist with module body and record types.
    *)
    this.InitializeWorklist();
   (*
    *  Emit struct declarations and static fields of body.
    *)
    classIx := 0;
    WHILE classIx < out.work.tide DO
      idDesc := out.work.a[classIx];
      WITH idDesc : Id.BlkId DO
          this.EmitModStatics(out, idDesc);
      | idDesc : Id.TypId DO
          IF idDesc.type IS Ty.Procedure THEN
            out.Blank();
            out.EmitPTypeDecl(idDesc);
          ELSE
            this.EmitRecordStruct(out, idDesc);
          END;
      END;
      INC(classIx);
    END;
   (*
    *  Emit struct declarations for referenced types.
    *)

   (*
    *  Emit code for procedures in this module.
    *)

   (*
    *  Clean up and chain to assembler if required.
    *)
    IF callApi THEN (* just produce a textual IL file *)
      out.Blank();
      out.Comment("end output produced by gpcp");
      out.Finish();
     (*
      *   Store the filename for the assembler.
      *)
      asmName := this.mod.pkgNm;
    END;
    (* FIXME: Set the output name for MSBuild *)
    this.MakeAbsName();
  END Emit;

(* ============================================================ *)
(* ============================================================ *)
END LlvmMaker.
(* ============================================================ *)
(* ============================================================ *)
