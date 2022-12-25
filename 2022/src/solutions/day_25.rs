use std::fs;
use std::path::PathBuf;

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

    let numbers = read_file(&input_path);
    println!("{}", first_puzzle(&numbers));
}

fn read_file(path: &str) -> Vec<Vec<u8>> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| line.as_bytes().to_vec())
        .collect()
}

fn test(test_input_path: &str) {
    let numbers = read_file(test_input_path);
    assert_eq!(first_puzzle(&numbers), "2=-1=0");
}

fn to_decimal(snafu: &[u8]) -> i64 {
    let mut number = 0;

    for (power, digit) in snafu.iter().rev().enumerate().rev() {
        number += i64::pow(5, power as u32)
            * match digit {
                b'=' => -2,
                b'-' => -1,
                n => (n - b'0') as i64,
            };
    }

    number
}

fn to_pental(mut decimal: i64) -> Vec<u8> {
    let mut pental = Vec::new();

    while decimal > 0 {
        pental.push((decimal % 5) as u8);
        decimal /= 5;
    }

    pental.iter().cloned().rev().collect()
}

fn to_snafu(decimal: i64) -> Vec<u8> {
    let pental = to_pental(decimal);

    let mut snafu = Vec::new();
    let mut carry = 0;

    for p in pental.iter().rev() {
        snafu.push(match p + carry {
            n @ (0 | 1 | 2) => b'0' + n,
            3 => b'=',
            4 => b'-',
            _ => b'0',
        });
        carry = (p + carry >= 3) as u8;
    }

    if carry > 0 {
        snafu.push(b'1');
    }

    snafu.iter().cloned().rev().collect()
}

fn first_puzzle(numbers: &[Vec<u8>]) -> String {
    let s = numbers.iter().map(|n| to_decimal(n)).sum();
    std::string::String::from_utf8(to_snafu(s)).unwrap()
}
