// we need this wrapper to convert number_literal to i32 
// since we don't have explicit casts yet
fn to_i32(x: i32): i32 { x; };


{ to_i32(150) + to_i32(50); };

{ to_i32(200); };
