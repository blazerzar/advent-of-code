use std::fs;
use std::path::PathBuf;

enum Outcome {
    Loss,
    Draw,
    Win,
}

enum HandShape {
    Rock,
    Paper,
    Scissors,
}

impl HandShape {
    fn new(symbol: &u8) -> HandShape {
        match symbol {
            b'A' | b'X' => HandShape::Rock,
            b'B' | b'Y' => HandShape::Paper,
            b'C' | b'Z' => HandShape::Scissors,
            _ => panic!("Invalid shape!"),
        }
    }

    fn from(opponent: &u8, outcome: &u8) -> (HandShape, HandShape) {
        let user = match outcome {
            b'X' => (((opponent - b'A') as i8 - 1) % 3 + 3) % 3,
            b'Y' => (opponent - b'A') as i8,
            b'Z' => ((opponent - b'A') as i8 + 1) % 3,
            _ => panic!("Invalid outcome!"),
        };

        let user = match user {
            0 => HandShape::Rock,
            1 => HandShape::Paper,
            2 => HandShape::Scissors,
            _ => panic!("Not possible!"),
        };

        let opponent = HandShape::new(opponent);

        (opponent, user)
    }

    fn score(&self) -> i32 {
        match *self {
            HandShape::Rock => 1,
            HandShape::Paper => 2,
            HandShape::Scissors => 3,
        }
    }

    fn outcome(opponent: &Self, user: &Self) -> Outcome {
        let opponent = opponent.score();
        let user = user.score();

        if opponent == user {
            Outcome::Draw
        } else if ((opponent - 2) % 3 + 3) % 3 + 1 == user {
            Outcome::Loss
        } else {
            Outcome::Win
        }
    }
}

const POINTS_LOSS: i32 = 0;
const POINTS_DRAW: i32 = 3;
const POINTS_WIN: i32 = 6;

pub fn solve() {
    let day = module_path!().split("::").last().unwrap();
    let day: u8 = day.split('_').last().unwrap().parse().unwrap();
    println!("Solving day {day}.");

    let input_path: PathBuf = ["inputs", &format!("day_{:02}.txt", day)].iter().collect();
    let test_input_path: PathBuf = ["inputs", &format!("day_{:02}_test.txt", day)]
        .iter()
        .collect();
    let input_path = input_path.display().to_string();
    let test_input_path = test_input_path.display().to_string();

    test(&test_input_path);

    let strategy = read_file(&input_path);
    println!("{}", first_puzzle(&strategy));
    println!("{}", second_puzzle(&strategy));
}

fn read_file(path: &str) -> Vec<(u8, u8)> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| (line.as_bytes()[0], line.as_bytes()[2]))
        .collect()
}

fn test(test_input_path: &str) {
    let strategy = read_file(test_input_path);
    assert!(first_puzzle(&strategy) == 15);
    println!("{}", second_puzzle(&strategy));
    assert!(second_puzzle(&strategy) == 12);
}

fn strategy_points(strategy: &[(HandShape, HandShape)]) -> i32 {
    strategy
        .iter()
        .map(
            |(opponent, user)| match HandShape::outcome(opponent, user) {
                Outcome::Loss => POINTS_LOSS + user.score(),
                Outcome::Draw => POINTS_DRAW + user.score(),
                Outcome::Win => POINTS_WIN + user.score(),
            },
        )
        .sum()
}

fn first_puzzle(strategy: &[(u8, u8)]) -> i32 {
    let strategy: Vec<(HandShape, HandShape)> = strategy
        .iter()
        .map(|(opponent, user)| (HandShape::new(opponent), HandShape::new(user)))
        .collect();
    strategy_points(&strategy)
}

fn second_puzzle(strategy: &[(u8, u8)]) -> i32 {
    let strategy: Vec<(HandShape, HandShape)> = strategy
        .iter()
        .map(|(opponent, outcome)| HandShape::from(opponent, outcome))
        .collect();
    strategy_points(&strategy)
}
