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

    let elves = read_file(&input_path);
    println!("{}", first_puzzle(&elves));
    println!("{}", second_puzzle(&elves));
}

fn read_file(path: &str) -> Vec<Vec<i32>> {
    let file = fs::read_to_string(path).unwrap();
    let elves: Vec<Vec<i32>> = file
        .trim()
        .split("\n\n")
        .map(|inventory| inventory.split('\n').map(|x| x.parse().unwrap()).collect())
        .collect();
    elves
}

fn test(test_input_path: &str) {
    let elves = read_file(test_input_path);
    assert!(first_puzzle(&elves) == 24000);
    assert!(second_puzzle(&elves) == 45000);
}

fn first_puzzle(elves: &[Vec<i32>]) -> i32 {
    let largest_inventory: i32 = elves
        .iter()
        .map(|inventory| inventory.iter().sum())
        .max()
        .unwrap();
    largest_inventory
}

fn second_puzzle(elves: &[Vec<i32>]) -> i32 {
    let mut inventories: Vec<i32> = elves
        .iter()
        .map(|inventory| inventory.iter().sum())
        .collect();
    inventories.sort();

    let top_three: i32 = inventories.iter().rev().take(3).sum();
    top_three
}
