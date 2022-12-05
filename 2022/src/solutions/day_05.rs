use std::collections::LinkedList;
use std::fs;
use std::path::PathBuf;

type Stack = LinkedList<u8>;
type Instruction = (usize, usize, usize);

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

    let (stacks, instructions) = read_file(&input_path);
    println!("{}", first_puzzle(&stacks, &instructions));
    println!("{}", second_puzzle(&stacks, &instructions));
}

fn read_file(path: &str) -> (Vec<Stack>, Vec<Instruction>) {
    let mut stacks: Vec<Stack> = Vec::new();

    let lines = fs::read_to_string(path).unwrap();
    let lines: Vec<&str> = lines.split("\n\n").collect();

    // Iterate over arrangement lines
    for line in lines[0].split('\n').map(|line| line.as_bytes()) {
        for i in (1..line.len()).step_by(4) {
            let stack_index = (i - 1) / 4;
            let cargo_crate = line[i];

            if stack_index >= stacks.len() {
                stacks.push(LinkedList::new());
            }

            // Empty space or row of stack numbers
            if cargo_crate < b'A' {
                continue;
            }

            stacks[stack_index].push_front(cargo_crate);
        }
    }

    // Iterate over instructions lines
    let instructions = lines[1]
        .split('\n')
        .map(|line| {
            let m: Vec<usize> = line
                .split(' ')
                .skip(1)
                .step_by(2)
                .map(|x| x.parse().unwrap())
                .collect();
            (m[0], m[1], m[2])
        })
        .collect();

    (stacks, instructions)
}

fn test(test_input_path: &str) {
    let (stacks, instructions) = read_file(test_input_path);
    assert!(first_puzzle(&stacks, &instructions) == "CMZ");
    assert!(second_puzzle(&stacks, &instructions) == "MCD");
}

fn get_top_crates(stacks: &[Stack]) -> String {
    String::from_utf8(
        stacks
            .iter()
            .filter(|stack| !stack.is_empty())
            .map(|stack| *stack.iter().last().unwrap())
            .collect(),
    )
    .unwrap()
}

fn first_puzzle(stacks: &[Stack], instructions: &[Instruction]) -> String {
    let mut stacks = stacks.to_vec();

    // Move crates according to instructions
    for (count, from, to) in instructions {
        for _ in 0..*count {
            let removed = stacks[*from - 1].pop_back().unwrap();
            stacks[*to - 1].push_back(removed);
        }
    }

    get_top_crates(&stacks)
}

fn second_puzzle(stacks: &[Stack], instructions: &[Instruction]) -> String {
    let mut stacks = stacks.to_vec();

    // Remove multiple crates without changing the order
    for (count, from, to) in instructions {
        let removed_index = stacks[*from - 1].len() - *count;
        let removed = stacks[*from - 1].split_off(removed_index);
        stacks[*to - 1].extend(removed);
    }

    get_top_crates(&stacks)
}
