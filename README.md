# readjava
Display information about the contents of Java virtual machine class format files.

Supports Java virtual machine class format files up to JDK 1.2.

### Usage
```bash
readjava <option(s)> java-class-file(s)
```
  
#### Options are:
| Flag              | Description                
| :---------------- | :----------
| `-a` `--all`      | Equivalent to: -cpfm
| `-c`              | Display class info
| `-p`              | Display constant pool content
| `-f`              | Display fields info
| `-m`              | Display methods info
| `-h` `--help`     | Display this information
| `-v` `--version`  | Display the version number of readjava
