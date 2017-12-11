

MODULE StringCompare;
  IMPORT 
         RTS,
         Console,
         java_lang,
         CPmain;

  CONST ok  = "match";
        no  = "no match";
        str = "Blah42";
  VAR 
      name : RTS.NativeString; 
      ltNm : RTS.NativeString;
      sObj : RTS.NativeObject;
      cArr : ARRAY 16 OF CHAR;
      cInt : INTEGER;
      cpStr : ARRAY 16 OF CHAR;
  BEGIN
      name := MKSTR(str);
     (*
      * This attempt works because String.equals() is not overloaded
      * This binds to the procedure matching
      * PROCEDURE (s : Sys.String)equals*(Sys.Object) : BOOLEAN
      *)
      Console.WriteString("Comparing NativeString with CP literal string");
      Console.WriteLn;
      IF name.equals(str) THEN 
        Console.WriteString(ok) 
      ELSE
        Console.WriteString(no) 
      END;
      Console.WriteLn;
      Console.WriteLn;

      (*
      * Conversions use built-in functions. Here is a non-standard one that converts
      * char-arrays to native strings. This works ...
      *)
      Console.WriteString("Comparing NativeString with explicitly converted CP literal");
      Console.WriteLn;
      IF name.equals(MKSTR("Blah42")) THEN 
        Console.WriteString(ok) 
      ELSE
        Console.WriteString(no) 
      END;
      Console.WriteLn();
      Console.WriteLn();

      (*
      * In the case of assigments (or non-overloaded method calls), the compiler can
      * work it out by itself without the MKSTR. Literal char arrays can be assigned to
      * objects or strings. This works.
      *)
      Console.WriteString("CP literal is implicitly converted in assign to NativeString");
      Console.WriteLn();
      ltNm := "Blah42";(* gpcp automatically converts the string to String *)
      IF name.equals(ltNm) THEN 
        Console.WriteString(ok) 
      ELSE
        Console.WriteString(no) 
      END;
      Console.WriteLn();
      Console.WriteLn();

      Console.WriteString("Virtual call on NativeString value");
      Console.WriteLn();
      IF ltNm.equals(name) THEN 
        Console.WriteString(ok) 
      ELSE
        Console.WriteString(no) 
      END;
      Console.WriteLn();
      Console.WriteLn();

      sObj := "Blah42"; (* gpcp automatically converts the string to Object *)
      Console.WriteString("Virtual call on NativeString value in NativeObject variable");
      Console.WriteLn();
      IF sObj.equals(name) THEN 
        Console.WriteString(ok) 
      ELSE
        Console.WriteString(no) 
      END;
      Console.WriteLn();
      Console.WriteLn();

      sObj := "Blah42"; (* gpcp automatically converts the string to Object *)
      Console.WriteString("Virtual call on NativeString value with NativeObject argument");
      Console.WriteLn();
      IF name.equals(sObj) THEN 
        Console.WriteString(ok) 
      ELSE
        Console.WriteString(no) 
      END;
      Console.WriteLn();
  END StringCompare.

