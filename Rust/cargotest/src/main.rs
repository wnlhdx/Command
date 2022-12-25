use std::io;
fn main() {
    println!("guess number");
    let mut guess=String::new();
    io::stdin().read_line(&mut guess).expect("wrong");
    println!("your number {}",guess);
}
