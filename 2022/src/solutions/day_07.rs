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

    let file_system = read_file(&input_path);
    println!("{}", first_puzzle(&file_system));
    println!("{}", second_puzzle(&file_system));
}

enum Node {
    File(u32),
    Directory(Vec<String>),
}

impl Node {
    fn size(&self, file_system: &HashMap<String, Node>) -> u32 {
        match &self {
            Node::File(s) => *s,
            Node::Directory(nodes) => nodes
                .iter()
                .map(|node| file_system.get(node).unwrap().size(file_system))
                .sum(),
        }
    }
}

fn build_file_system(terminal_output: &Vec<Vec<String>>) -> HashMap<String, Node> {
    let mut file_system = HashMap::new();
    file_system.insert(String::from("/"), Node::Directory(Vec::new()));
    let mut cwd: PathBuf = ["/"].iter().collect();

    for command_output in terminal_output {
        let command: Vec<&str> = command_output[0].split_whitespace().collect();
        let output: Vec<(&str, &str)> = command_output[1..]
            .iter()
            .map(|line| {
                let line: Vec<&str> = line.split_whitespace().collect();
                (line[0], line[1])
            })
            .collect();

        match command[..] {
            ["cd", "/"] => cwd = ["/"].iter().collect(),
            ["cd", ".."] => {
                cwd.pop();
            }
            ["cd", dir] => cwd.push(dir),
            ["ls"] => {
                for (size, name) in output {
                    // Add file to currect directory
                    let cwd_nodes;
                    if let Some(Node::Directory(nodes)) =
                        file_system.get_mut(&cwd.display().to_string())
                    {
                        cwd_nodes = nodes;
                    } else {
                        continue;
                    };

                    cwd.push(name);
                    let path = cwd.display().to_string();
                    cwd.pop();

                    // Add file to file system
                    cwd_nodes.push(path.clone());
                    let new_node = if size == "dir" {
                        Node::Directory(Vec::new())
                    } else {
                        Node::File(size.parse().unwrap())
                    };

                    file_system.insert(path, new_node);
                }
            }
            _ => panic!("Invalid command!"),
        };
    }

    file_system
}

fn read_file(path: &str) -> HashMap<String, Node> {
    let terminal_output = fs::read_to_string(path)
        .unwrap()
        .split("$ ")
        .map(|command| {
            command
                .split('\n')
                .map(String::from)
                .filter(|line| !line.is_empty())
                .collect()
        })
        .filter(|command: &Vec<String>| !command.is_empty())
        .collect();

    build_file_system(&terminal_output)
}

fn test(test_input_path: &str) {
    let file_system = read_file(test_input_path);
    assert!(first_puzzle(&file_system) == 95437);
    assert!(second_puzzle(&file_system) == 24933642);
}

fn first_puzzle(file_system: &HashMap<String, Node>) -> u32 {
    file_system
        .iter()
        .map(|(_, node)| {
            if let Node::Directory(_) = node {
                node.size(file_system)
            } else {
                0
            }
        })
        .filter(|&size| size <= 100000)
        .sum()
}

fn second_puzzle(file_system: &HashMap<String, Node>) -> u32 {
    let sizes: HashMap<String, u32> = file_system
        .iter()
        .map(|(path, node)| {
            (
                String::from(path),
                if let Node::Directory(_) = node {
                    node.size(file_system)
                } else {
                    0
                },
            )
        })
        .filter(|(_, size)| *size > 0)
        .collect();

    let total_space = 70000000;
    let required_space = 30000000;
    let available_space = total_space - sizes.get(&String::from("/")).unwrap();

    let mut sizes: Vec<u32> = sizes
        .iter()
        .filter(|(_, &size)| available_space + size >= required_space)
        .map(|(_, &size)| size)
        .collect();
    sizes.sort();
    sizes[0]
}
