x: u32;
y: u8;

fn foo[T](a: T, b: T) {

};

foo(x, y); 
// T gets bound to typeof(x) aka u32 at the first argument
// the second argument is u8, u32 can't be implicitly casted to it so we fail
