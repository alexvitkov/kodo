// number literals must implicitly cast down to the primitive numeric types

fn foo1(x: i8)  { };
fn foo2(x: i16) { };
fn foo3(x: i32) { };
fn foo4(x: i64) { };
fn foo5(x: u8)  { };
fn foo6(x: u16) { };
fn foo7(x: u32) { };
fn foo8(x: u64) { };

foo1(100); // 100, which is of type number_literal gets casted to i8,
foo2(100); // and to i16,
foo3(100); // ...
foo4(100);
foo5(100);
foo6(100);
foo7(100);
foo8(100);
