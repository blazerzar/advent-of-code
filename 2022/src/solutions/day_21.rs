use std::collections::HashMap;
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

    let jobs = read_file(&input_path);
    println!("{}", first_puzzle(&jobs));
    println!("{}", second_puzzle(&jobs));
}

#[derive(Clone)]
enum Operation {
    Sum,
    Sub,
    Prod,
    Div,
}

enum Job {
    Number(i64),
    Operation(String, String, Operation),
}

impl Job {
    fn from_operation(monkey_a: String, monkey_b: String, operation: u8) -> Job {
        let op = match operation {
            b'+' => Operation::Sum,
            b'-' => Operation::Sub,
            b'*' => Operation::Prod,
            b'/' => Operation::Div,
            _ => panic!("Invalid operation!"),
        };

        Job::Operation(monkey_a, monkey_b, op)
    }
}

fn read_file(path: &str) -> HashMap<String, Job> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let line = line.split(": ").collect::<Vec<_>>();
            let monkey = line[0].to_string();

            let job = line[1].split_whitespace().collect::<Vec<_>>();
            let job = if job.len() == 1 {
                Job::Number(job[0].parse().unwrap())
            } else {
                Job::from_operation(job[0].to_string(), job[2].to_string(), job[1].as_bytes()[0])
            };

            (monkey, job)
        })
        .collect()
}

fn test(test_input_path: &str) {
    let jobs = read_file(test_input_path);
    assert_eq!(first_puzzle(&jobs), 152);
    assert_eq!(second_puzzle(&jobs), 301);
}

// Binary tree nodes
struct Node {
    value: Option<i64>,
    parent: Option<String>,
    children: Option<(String, String, Operation)>,
    from_humn: bool,
}

fn create_tree(jobs: &HashMap<String, Job>) -> HashMap<String, Node> {
    // Create tree
    let mut nodes: HashMap<String, Node> = jobs
        .keys()
        .map(|name| {
            (
                name.to_owned(),
                Node {
                    value: None,
                    parent: None,
                    children: None,
                    from_humn: false,
                },
            )
        })
        .collect();

    // Set up tree value and relations
    for (name, job) in jobs {
        let node = nodes.get_mut(name).unwrap();

        match job {
            Job::Number(n) => {
                node.value = Some(*n);
                node.from_humn = name == "humn";
            }
            Job::Operation(monkey_a, monkey_b, op) => {
                node.children = Some((monkey_a.to_owned(), monkey_b.to_owned(), op.clone()));

                let monkey_a = nodes.get_mut(monkey_a).unwrap();
                monkey_a.parent = Some(name.to_owned());

                let monkey_b = nodes.get_mut(monkey_b).unwrap();
                monkey_b.parent = Some(name.to_owned());
            }
        }
    }

    nodes
}

fn calculate_value(node: &str, nodes: &mut HashMap<String, Node>) -> i64 {
    let children = {
        let node = nodes.get_mut(node).unwrap();
        if node.value.is_some() {
            return node.value.unwrap();
        }
        node.children.clone()
    };

    let (monkey_a, monkey_b, op) = children.unwrap();

    // Calculate children values and use them for current node value calculation
    let monkey_a_value = calculate_value(&monkey_a, nodes);
    let monkey_b_value = calculate_value(&monkey_b, nodes);

    // Check if this node is calculated from user input
    let from_humn = {
        let monkey_a = nodes.get(&monkey_a).unwrap().from_humn;
        let monkey_b = nodes.get(&monkey_b).unwrap().from_humn;
        monkey_a || monkey_b
    };

    let value = match op {
        Operation::Sum => monkey_a_value + monkey_b_value,
        Operation::Sub => monkey_a_value - monkey_b_value,
        Operation::Prod => monkey_a_value * monkey_b_value,
        Operation::Div => monkey_a_value / monkey_b_value,
    };

    // Save calculations and return value
    let node = nodes.get_mut(node).unwrap();
    node.value = Some(value);
    node.from_humn = from_humn;

    value
}

fn first_puzzle(jobs: &HashMap<String, Job>) -> i64 {
    let mut tree = create_tree(jobs);
    calculate_value("root", &mut tree)
}

fn child_value(value: i64, monkey: i64, op: Operation, monkey_left: bool) -> i64 {
    match (op, monkey_left) {
        (Operation::Sum, _) => value - monkey,
        (Operation::Sub, true) => monkey - value,
        (Operation::Sub, false) => value + monkey,
        (Operation::Prod, _) => value / monkey,
        (Operation::Div, true) => monkey / value,
        (Operation::Div, false) => value * monkey,
    }
}

fn calculate_humn(node: &str, nodes: &mut HashMap<String, Node>) {
    let node = nodes.get_mut(node).unwrap();
    if node.children.is_none() {
        return;
    }

    let node_value = node.value.unwrap();
    let (monkey_a, monkey_b, op) = node.children.clone().unwrap();

    // Get both children's values
    let (value_a, humn_a) = {
        let monkey_a = nodes.get_mut(&monkey_a).unwrap();
        (monkey_a.value.unwrap(), monkey_a.from_humn)
    };
    let value_b = nodes.get_mut(&monkey_b).unwrap().value.unwrap();

    // Calculate user input to achieve given value in parent node
    if humn_a {
        let node = nodes.get_mut(&monkey_a).unwrap();
        node.value = Some(child_value(node_value, value_b, op, false));
        calculate_humn(&monkey_a, nodes);
    } else {
        let node = nodes.get_mut(&monkey_b).unwrap();
        node.value = Some(child_value(node_value, value_a, op, true));
        calculate_humn(&monkey_b, nodes);
    }
}

fn second_puzzle(jobs: &HashMap<String, Job>) -> i64 {
    let mut tree = create_tree(jobs);
    calculate_value("root", &mut tree);

    let root = tree.get("root").unwrap();
    let (monkey_a, monkey_b, _) = root.children.clone().unwrap();

    let (value_a, humn_a) = {
        let monkey_a = tree.get_mut(&monkey_a).unwrap();
        (monkey_a.value.unwrap(), monkey_a.from_humn)
    };
    let value_b = tree.get_mut(&monkey_b).unwrap().value.unwrap();

    // Calculate user input to achieve equality
    if humn_a {
        tree.get_mut(&monkey_a).unwrap().value = Some(value_b);
        calculate_humn(&monkey_a, &mut tree);
    } else {
        tree.get_mut(&monkey_b).unwrap().value = Some(value_a);
        calculate_humn(&monkey_b, &mut tree);
    }

    tree.get("humn").unwrap().value.unwrap()
}
