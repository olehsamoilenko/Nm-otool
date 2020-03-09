# Nm-otool

# Testing
To run tests type:<br>
```bash test.sh```<br>
To run test on specific binary, type:<br>
```bash test.sh binary```

# List of flags
## ft_nm
```
# -n     Sort numerically rather than alphabetically.
-j     Just display the symbol names (no value or type).
-r     Sort in reverse order.
-x     Display  the  symbol  table  entry's  fields in hexadecimal, along with the name as a string.
-p     Don't sort; display in symbol-table order.
```

## ft_otool
```
# -a     Display the archive header, if the file is an archive.
# -h     Display the Mach header.
# -f     Display the universal headers.
# -d     Display the contents of the (__DATA,__data) section.
# -t     Display  the  contents of the (__TEXT,__text) section.
# -G     Display the data in code table.
```
