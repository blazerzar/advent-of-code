use std::collections::HashSet;
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

    let rucksacks = read_file(&input_path);
    println!("{}", first_puzzle(&rucksacks));
    println!("{}", second_puzzle(&rucksacks));
}

fn read_file(path: &str) -> Vec<String> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(String::from)
        .collect::<Vec<String>>()
}

fn test(test_input_path: &str) {
    let rucksacks = read_file(test_input_path);
    assert!(first_puzzle(&rucksacks) == 157);
    assert!(second_puzzle(&rucksacks) == 70);
}

fn priority(item: u8) -> i32 {
    (if item < 91 {
        item - b'A' + 27
    } else {
        item - b'a' + 1
    }) as i32
}

fn first_puzzle(rucksacks: &Vec<String>) -> i32 {
    let mut priorities_sum = 0;

    for rucksack in rucksacks {
        let mut first_items: HashSet<u8> = HashSet::new();
        first_items.extend(rucksack[..(rucksack.len() / 2)].as_bytes().iter());
        let mut second_items: HashSet<u8> = HashSet::new();
        second_items.extend(rucksack[(rucksack.len() / 2)..].as_bytes().iter());

        let incorrect = first_items.intersection(&second_items).next().unwrap();
        priorities_sum += priority(*incorrect);
    }

    priorities_sum
}

fn second_puzzle(rucksacks: &Vec<String>) -> i32 {
    let mut priorities_sum = 0;

    for i in (0..rucksacks.len()).step_by(3) {
        let group = &rucksacks[i..i + 3];

        let mut first: HashSet<u8> = HashSet::new();
        first.extend(group[0].as_bytes().iter());
        let mut second: HashSet<u8> = HashSet::new();
        second.extend(group[1].as_bytes().iter());
        let mut third: HashSet<u8> = HashSet::new();
        third.extend(group[2].as_bytes().iter());

        first.retain(|x| second.contains(x));
        first.retain(|x| third.contains(x));

        let badge = first.iter().next().unwrap();
        priorities_sum += priority(*badge);
    }

    priorities_sum
}
