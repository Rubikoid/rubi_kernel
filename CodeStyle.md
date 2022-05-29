# Code style

Based on google styling.

## Headers and sources

Headers are in include/kernel, so can be included from kernel/

Source are in src/

Every source file must be associatied with .h header

Define for headers must be like `<PROJECT>_<PATH>_<FILE>_H_`

## Cases

Type names must be CamelCase: `MyExcitingClass, MyExcitingEnum`

Varible names must be snake case: `a_local_variable, a_struct_data_member, a_class_data_member_`

Constants must be lowerCamelCase:

```c
const int kDaysInAWeek = 7;
const int kAndroid8_0_0 = 24;  // Android 8.0.0
```

Functions must be lower snake case:

```c
add_table_entry()
delete_url()
open_file_or_die()
```

## Remark on CamelCase

If you have two higher case symbols -> insert underscore
