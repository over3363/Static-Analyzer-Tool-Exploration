| # | Vulnerability | Location | Description |
| --- | --- | --- | --- |
| 1 | Uninitialized Variable | main() | user_choice declared but never initialized before use |
| 2 | Buffer Overflow | create_profile() | name field is 16 bytes, input can exceed this |
| 3 | Unsafe Function | create_profile(), print_fortune_banner() | strcpy() and strcat() without bounds checking |
| 4 | Type Mismatch | create_profile() | Casting char* return of strcpy to int |
| 5 | Format String | print_fortune_banner() | User input passed directly to printf() as format |
| 6 | Integer Overflow | calculate_lucky_number() | Multiplying large integers causes overflow |
| 7 | Off-by-One | get_fortune_index() | % (max + 1) allows index equal to array size |
| 8 | Double Free | demonstrate_memory_bugs(), main() | Same pointer freed twice |
| 9 | Null Pointer Dereference | main() | Accessing member of NULL pointer |
| 10 | Use After Free | demonstrate_memory_bugs(), main() | Accessing memory after free() |