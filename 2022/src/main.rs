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
        5 => solutions::day_05::solve(),
        6 => solutions::day_06::solve(),
        7 => solutions::day_07::solve(),
        8 => solutions::day_08::solve(),
        9 => solutions::day_09::solve(),
        10 => solutions::day_10::solve(),
        11 => solutions::day_11::solve(),
        12 => solutions::day_12::solve(),
        _ => panic!("Invalid day!"),
    }
}
