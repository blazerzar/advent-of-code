use std::collections::LinkedList;
use std::fs;
use std::num::ParseIntError;
use std::path::PathBuf;

#[derive(Clone)]
struct Monkey {
    items: LinkedList<u64>,
    operation: (u8, Result<u64, ParseIntError>),
    test: u64,
    next_monkey: Vec<usize>,
}

impl Monkey {
    fn next_monkey(&self, item: u64, reducer: u64, modulo: u64) -> (usize, u64) {
        // Calculate new worry level
        let item = match self.operation.1 {
            Ok(v) => [v * item, v + item],
            Err(_) => [item * item, item + item],
        }[(self.operation.0 - b'*') as usize];
        let item = item % modulo / reducer;

        (self.next_monkey[(item % self.test != 0) as usize], item)
    }
}

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

    let monkeys = read_file(&input_path);
    println!("{}", first_puzzle(&monkeys));
    println!("{}", second_puzzle(&monkeys));
}

fn read_file(path: &str) -> Vec<Monkey> {
    // Split by two new lines to get each monkey
    fs::read_to_string(path)
        .unwrap()
        .split("\n\n")
        .map(|monkey_input| {
            // Split by new line and skip the first one, to just get the data
            let monkey_input = monkey_input
                .split('\n')
                .skip(1)
                .map(|line| line.split(": ").nth(1).unwrap())
                .collect::<Vec<_>>();
            let starting_items = monkey_input[0]
                .split(", ")
                .map(|x| x.parse().unwrap())
                .collect::<LinkedList<_>>();
            let operation = monkey_input[1]
                .split("= old ")
                .nth(1)
                .unwrap()
                .split(' ')
                .collect::<Vec<_>>();
            let test = monkey_input[2]
                .split("by ")
                .nth(1)
                .unwrap()
                .parse()
                .unwrap();
            let next_monkey = monkey_input[3..]
                .iter()
                .map(|next_input| next_input.split(' ').last().unwrap().parse().unwrap())
                .collect::<Vec<_>>();

            Monkey {
                items: starting_items,
                operation: (operation[0].as_bytes()[0], operation[1].parse()),
                test,
                next_monkey,
            }
        })
        .collect()
}

fn test(test_input_path: &str) {
    let monkeys = read_file(test_input_path);
    let modulo = monkeys.iter().map(|monkey| monkey.test).product();

    assert!(monkeys[0].next_monkey(79, 3, modulo) == (3, 500));
    assert!(monkeys[1].next_monkey(54, 3, modulo) == (0, 20));
    assert!(monkeys[2].next_monkey(79, 3, modulo) == (1, 2080));
    assert!(monkeys[3].next_monkey(74, 3, modulo) == (1, 25));
    assert!(first_puzzle(&monkeys) == 10605);
    assert!(second_puzzle(&monkeys) == 2713310158);
}

fn simulate_rounds(monkeys: &[Monkey], rounds: usize, reducer: u64) -> u64 {
    // Count number of item inspections for each monkey
    let mut inspections = vec![0; monkeys.len()];
    let mut monkeys = monkeys.to_vec();

    let modulo = monkeys.iter().map(|monkey| monkey.test).product();

    for _ in 0..rounds {
        for i in 0..monkeys.len() {
            while !monkeys[i].items.is_empty() {
                let monkey = &mut monkeys[i];
                let item = monkey.items.pop_front().unwrap();

                let (next_monkey, item) = monkey.next_monkey(item, reducer, modulo);
                monkeys[next_monkey].items.push_back(item);

                inspections[i] += 1;
            }
        }
    }

    inspections.sort();
    let top_two = inspections.iter().rev().take(2).collect::<Vec<_>>();
    top_two[0] * top_two[1]
}

fn first_puzzle(monkeys: &[Monkey]) -> u64 {
    simulate_rounds(monkeys, 20, 3)
}

fn second_puzzle(monkeys: &[Monkey]) -> u64 {
    simulate_rounds(monkeys, 10000, 1)
}
