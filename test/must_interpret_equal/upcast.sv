// we need this wrapper to convert number_literal to i32 
// since we don't have explicit casts yet
fn to_i32(x: i32): i32 { x; };

fn add(x: i32, y: i16): i32 { x + y; };


{ add(150, 50); };

{ to_i32(200); };
