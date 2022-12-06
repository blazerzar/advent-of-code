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

    let stream = read_file(&input_path);
    println!("{}", first_puzzle(&stream));
    println!("{}", second_puzzle(&stream));
}

fn read_file(path: &str) -> String {
    fs::read_to_string(path).unwrap().trim().to_string()
}

fn test(test_input_path: &str) {
    let streams = read_file(test_input_path);
    let first_puzzle_results = [7, 5, 6, 10, 11];
    let second_puzzle_results = [19, 23, 23, 29, 26];

    for (i, stream) in streams.split('\n').enumerate() {
        assert!(first_puzzle(stream) == first_puzzle_results[i]);
        assert!(second_puzzle(stream) == second_puzzle_results[i]);
    }
}

fn first_distinct_characters(stream: &str, n: usize) -> usize {
    let mut characters = [0; 256];
    let mut different_characters: usize = 0;

    let bytes = stream.as_bytes();
    for (i, c) in bytes.iter().enumerate() {
        different_characters += (characters[*c as usize] == 0) as usize;
        characters[*c as usize] += 1;

        if different_characters >= n {
            return i + 1;
        }

        if i >= n - 1 {
            let first_char = &mut characters[bytes[i - (n - 1)] as usize];
            *first_char -= 1;
            different_characters -= (*first_char == 0) as usize;
        }
    }

    stream.len()
}

fn first_puzzle(stream: &str) -> usize {
    first_distinct_characters(stream, 4)
}

fn second_puzzle(stream: &str) -> usize {
    first_distinct_characters(stream, 14)
}
