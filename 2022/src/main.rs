use std::env;

mod solutions;

fn main() {
    let args: Vec<String> = env::args().collect();
    let day: u8 = args.last().unwrap().parse().unwrap();

    match day {
        1 => solutions::day_01::solve(),
        2 => solutions::day_02::solve(),
        3 => solutions::day_03::solve(),
        4 => solutions::day_04::solve(),
        _ => panic!("Invalid day!"),
    }
}
