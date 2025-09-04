@echo off
echo Generating OpenNGC files...

echo Step 1: Generating openngc-entries.csv...
..\win\build\bin\gawk.exe -F; -f openngc-entries-csv.awk NGC.csv > openngc-entries.csv

echo Step 2: Generating openngc-names.csv...
..\win\build\bin\gawk.exe -F; -f openngc-names-csv.awk NGC.csv > openngc-names.csv

echo Step 3: Generating openngc-entries.c...
..\win\build\bin\gawk.exe -F; -f openngc-entries-c.awk openngc-entries.csv > openngc-entries.c

echo Step 4: Generating openngc-names.c...
..\win\build\bin\gawk.exe -F; -f openngc-names-c.awk openngc-names.csv > openngc-names.c

echo Done!
echo.
echo Generated files:
dir openngc-*.c openngc-*.csv
