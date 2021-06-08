fn bar(x: i32, y: i32) {};

fn foo[T1, T2](a: T1, b: T2) {
    bar(a, b); // this should resolve to the definition of bar(i32,i32)
};


x: i32;
y: i32;
foo(x, y);
