MODULE MkOver;
  IMPORT CPmain, GPText, GPTextFiles;

  VAR name : ARRAY 8 OF CHAR;
      indx : INTEGER;
      file : GPTextFiles.FILE;

BEGIN
  name := "	c";
  file := GPTextFiles.createFile("TooBig.cp");
  GPText.WriteString(file, "MODULE TooBig;"); GPText.WriteLn(file);
  GPText.WriteString(file, "  IMPORT CPmain, Console;"); GPText.WriteLn(file);
  GPText.WriteString(file, "  CONST"); GPText.WriteLn(file);
  FOR indx := 0 TO 10000 DO
    GPText.WriteString(file, name);
    GPText.WriteInt(file, indx, 1);
    GPText.WriteString(file, "*	 =");
    GPText.WriteInt(file, indx, 6);
    GPText.Write(file, ";");
    GPText.WriteLn(file);
  END;
  GPText.WriteString(file, "BEGIN"); GPText.WriteLn(file);
  GPText.WriteString(file, '  Console.WriteString("Not TOO Big!");'); GPText.WriteLn(file);
  GPText.WriteString(file, "END TooBig."); GPText.WriteLn(file);
  GPTextFiles.CloseFile(file);
END MkOver.

