use std::cmp::Ordering;
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

    let packet_pairs = read_file(&input_path);
    println!("{}", first_puzzle(&packet_pairs));
    println!("{}", second_puzzle(&packet_pairs));
}

#[derive(Debug, Clone, PartialEq, Eq)]
enum Packet {
    Integer(u32),
    List(Vec<Packet>),
}

impl Packet {
    fn from(packet_data: &str) -> Packet {
        let bytes = packet_data.as_bytes();
        match bytes[0] {
            b'[' => {
                let mut start = 1;
                let mut level = 0;
                let mut elements = Vec::new();

                for (i, &c) in bytes.iter().enumerate().skip(1) {
                    if c == b'[' {
                        level += 1;
                    } else if c == b']' && level > 0 {
                        level -= 1;
                    } else if c == b',' && level == 0 || c == b']' && start < i {
                        elements.push(Packet::from(&packet_data[start..i]));
                        start = i + 1;
                    }
                }

                Packet::List(elements)
            }
            _ => Packet::Integer(packet_data.parse().unwrap()),
        }
    }

    fn to_list(&self) -> Packet {
        match self {
            Packet::List(v) => Packet::List(v.clone()),
            Packet::Integer(_) => Packet::List(vec![self.clone()]),
        }
    }
}

impl PartialOrd for Packet {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Packet {
    fn cmp(&self, other: &Self) -> Ordering {
        match (self, other) {
            (Packet::Integer(left), Packet::Integer(right)) => left.cmp(right),
            (Packet::List(left), Packet::List(right)) => {
                for (left, right) in left.iter().zip(right.iter()) {
                    let cmp_res = left.cmp(right);
                    if cmp_res.is_ne() {
                        return cmp_res;
                    }
                }

                left.len().cmp(&right.len())
            }
            (Packet::Integer(_), Packet::List(_)) => self.to_list().cmp(other),
            (Packet::List(_), Packet::Integer(_)) => self.cmp(&other.to_list()),
        }
    }
}

fn read_file(path: &str) -> Vec<(Packet, Packet)> {
    fs::read_to_string(path)
        .unwrap()
        .split("\n\n")
        .map(|pair| {
            let packets = pair.split('\n').map(Packet::from).collect::<Vec<_>>();
            (packets[0].clone(), packets[1].clone())
        })
        .collect()
}

fn test(test_input_path: &str) {
    let packet_pairs = read_file(test_input_path);
    assert!(first_puzzle(&packet_pairs) == 13);
    assert!(second_puzzle(&packet_pairs) == 140);
}

fn first_puzzle(packet_pairs: &[(Packet, Packet)]) -> usize {
    packet_pairs
        .iter()
        .map(|(left, right)| left.cmp(right))
        .enumerate()
        .filter(|(_, ordering)| ordering.is_lt())
        .map(|(i, _)| i + 1)
        .sum()
}

fn second_puzzle(packet_pairs: &[(Packet, Packet)]) -> usize {
    // Create a vector of all packets and add two divider packets
    let divider_packets = vec![
        Packet::List(vec![Packet::List(vec![Packet::Integer(2)])]),
        Packet::List(vec![Packet::List(vec![Packet::Integer(6)])]),
    ];
    let mut packets = packet_pairs
        .iter()
        .map(|(left, _)| left)
        .chain(packet_pairs.iter().map(|(_, right)| right))
        .chain(divider_packets.iter())
        .collect::<Vec<_>>();
    packets.sort();

    // Find divider packets
    let divider_packets_indices = packets
        .iter()
        .enumerate()
        .map(|(i, &p)| (i + 1, p))
        .filter(|(_, p)| divider_packets.contains(p))
        .map(|(i, _)| i)
        .collect::<Vec<_>>();
    divider_packets_indices[0] * divider_packets_indices[1]
}
