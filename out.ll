@str_0 = internal constant [13 x i8] c"Hello world!\00"
declare i32 @printf(i8*, ...)
define i32 @main() {
entry:
  %0 = getelementptr [13 x i8], [13 x i8]* @str_0, i32 0, i32 0
  call i32 (i8*, ...) @printf(i8* %0)
  ret i32 0
}
