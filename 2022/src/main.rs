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
        13 => solutions::day_13::solve(),
        14 => solutions::day_14::solve(),
        15 => solutions::day_15::solve(),
        16 => solutions::day_16::solve(),
        17 => solutions::day_17::solve(),
        18 => solutions::day_18::solve(),
        19 => solutions::day_19::solve(),
        20 => solutions::day_20::solve(),
        21 => solutions::day_21::solve(),
        22 => solutions::day_22::solve(),
        23 => solutions::day_23::solve(),
        24 => solutions::day_24::solve(),
        25 => solutions::day_25::solve(),
        _ => panic!("Invalid day!"),
    }
}
