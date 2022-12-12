use std::cmp::Ordering;
use std::collections::{BinaryHeap, HashSet};
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

    let map = read_file(&input_path);
    println!("{}", first_puzzle(&map));
    println!("{}", second_puzzle(&map));
}

fn read_file(path: &str) -> Vec<Vec<u8>> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| line.as_bytes().to_vec())
        .collect()
}

fn test(test_input_path: &str) {
    let map = read_file(test_input_path);
    assert!(first_puzzle(&map) == 31);
    assert!(second_puzzle(&map) == 29);
}

const NEIGHBORS: [(isize, isize); 4] = [(-1, 0), (0, -1), (0, 1), (1, 0)];

#[derive(Eq, PartialEq)]
struct State {
    distance: usize,
    position: (usize, usize),
}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        other
            .distance
            .cmp(&self.distance)
            .then_with(|| self.position.cmp(&other.position))
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

type Position = (usize, usize);

fn find_shortest_path(
    map: &[Vec<u8>],
    start: Position,
    is_end: &dyn Fn(Position) -> bool,
) -> Option<usize> {
    let mut visited = HashSet::new();
    let mut q = BinaryHeap::new();
    q.push(State {
        distance: 0,
        position: start,
    });

    while let Some(State { distance, position }) = q.pop() {
        if is_end(position) {
            return Some(distance);
        }

        // We already found shortest path to visited positions
        if visited.contains(&position) {
            continue;
        }
        visited.insert(position);

        let (x, y) = position;
        for (dy, dx) in NEIGHBORS {
            let (x_, y_) = (x as isize + dx, y as isize + dy);
            if y_ < 0 || x_ < 0 {
                continue;
            }
            let (x_, y_) = (x_ as usize, y_ as usize);
            if y_ >= map.len() || x_ >= map[y_].len() {
                continue;
            }

            // Check if move is valid
            if map[y_][x_] > map[y][x] + 1 {
                continue;
            }

            // Add new state to the priority queue
            q.push(State {
                distance: distance + 1,
                position: (x_, y_),
            });
        }
    }

    None
}

fn first_puzzle(map: &[Vec<u8>]) -> usize {
    // Find start and end
    let mut start = (0, 0);
    let mut end = (0, 0);
    for (y, line) in map.iter().enumerate() {
        for (x, height) in line.iter().enumerate() {
            if *height == b'S' {
                start = (x, y);
            } else if *height == b'E' {
                end = (x, y);
            }
        }
    }

    let mut map = map.to_vec();
    map[start.1][start.0] = b'a';
    map[end.1][end.0] = b'z';

    find_shortest_path(&map, start, &(|position| position == end)).unwrap()
}

fn second_puzzle(map: &[Vec<u8>]) -> usize {
    // Find end and reverse map
    let mut map = map.to_vec();
    let mut ends = HashSet::new();
    let mut end = (0, 0);
    for (y, line) in map.iter_mut().enumerate() {
        for (x, height) in line.iter_mut().enumerate() {
            if *height == b'S' {
                *height = b'z';
            } else if *height == b'E' {
                end = (x, y);
                *height = b'a';
            } else {
                *height = b'z' - (*height - b'a');
            }

            if *height == b'z' {
                ends.insert((x, y));
            }
        }
    }

    find_shortest_path(&map, end, &(|position| ends.contains(&position))).unwrap()
}
