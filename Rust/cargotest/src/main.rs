use std::io;
use rand::Rng;
fn main() {
    println!("guess number");
    let mut guess=String::new();
    let number = rand::thread_rng().gen_range(1..=100);
    io::stdin().read_line(&mut guess).expect("wrong");
    println!("your number {}",guess);
    println!("number: {}",number);
}
