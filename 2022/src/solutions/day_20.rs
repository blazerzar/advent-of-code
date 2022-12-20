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

    let encrypted = read_file(&input_path);
    println!("{}", first_puzzle(&encrypted));
    println!("{}", second_puzzle(&encrypted));
}

fn read_file(path: &str) -> Vec<i64> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| line.parse().unwrap())
        .collect()
}

fn test(test_input_path: &str) {
    let encrypted = read_file(test_input_path);
    assert_eq!(first_puzzle(&encrypted), 3);
    assert_eq!(second_puzzle(&encrypted), 1623178306);
}

fn mix(array: &mut Vec<(usize, i64)>) {
    for i in 0..array.len() {
        let index = array
            .iter()
            .enumerate()
            .find(|(_, (j, _))| *j == i)
            .unwrap()
            .0;

        let pair = array.remove(index);
        let l = array.len() as i64;
        let mixed_index = ((index as i64 + pair.1) % l + l) % l;
        array.insert(mixed_index as usize, pair);
    }
}

fn coordinates_sum(array: &[(usize, i64)]) -> i64 {
    let zero_index = array
        .iter()
        .enumerate()
        .find(|(_, (_, n))| *n == 0)
        .unwrap()
        .0;

    [1000, 2000, 3000]
        .iter()
        .map(|offset| array[(zero_index + offset) % array.len()].1)
        .sum()
}

fn first_puzzle(encrypted: &[i64]) -> i64 {
    let mut mixed = encrypted.iter().cloned().enumerate().collect::<Vec<_>>();
    mix(&mut mixed);
    coordinates_sum(&mixed)
}

fn second_puzzle(encrypted: &[i64]) -> i64 {
    let mut mixed = encrypted
        .iter()
        .cloned()
        .map(|n| n * 811589153)
        .enumerate()
        .collect::<Vec<_>>();

    for _ in 0..10 {
        mix(&mut mixed);
    }

    coordinates_sum(&mixed)
}
