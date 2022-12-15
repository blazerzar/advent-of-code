use regex::Regex;
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

    let sensors = read_file(&input_path);
    println!("{}", first_puzzle(&sensors, 2000000));
    println!("{}", second_puzzle(&sensors, 4000000));
}

fn read_file(path: &str) -> Vec<(Position, Position)> {
    let re = Regex::new(r"^.* x=(-?\d+), y=(-?\d+): .* x=(-?\d+), y=(-?\d+)$").unwrap();
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let cap = re.captures(line).unwrap();
            (
                (cap[1].parse().unwrap(), cap[2].parse().unwrap()),
                (cap[3].parse().unwrap(), cap[4].parse().unwrap()),
            )
        })
        .collect()
}

fn test(test_input_path: &str) {
    let sensors = read_file(test_input_path);
    assert_eq!(first_puzzle(&sensors, 10), 26);
    assert_eq!(second_puzzle(&sensors, 20), 56000011);
}

type Position = (i32, i32);

fn first_puzzle(sensors: &[(Position, Position)], y: i32) -> usize {
    let mut clear = HashSet::new();

    for ((sensor_x, sensor_y), (beacon_x, beacon_y)) in sensors {
        let distance = (sensor_x - beacon_x).abs() + (sensor_y - beacon_y).abs();
        let distance_line = (sensor_y - y).abs();

        if distance_line <= distance {
            let remaining = distance - distance_line;
            for i in 0..(remaining + 1) {
                clear.insert(sensor_x - i);
                clear.insert(sensor_x + i);
            }

            if *beacon_y == y {
                clear.remove(beacon_x);
            }
        }
    }

    clear.len()
}

fn second_puzzle(sensors: &[(Position, Position)], max: i32) -> u64 {
    for ((sensor_x, sensor_y), (beacon_x, beacon_y)) in sensors {
        let distance = (sensor_x - beacon_x).abs() + (sensor_y - beacon_y).abs();

        // Get possible beacons just out of reach
        let mut edge = HashSet::new();
        for dx in 0..(distance + 2) {
            let dy = distance + 1 - dx;
            edge.insert((sensor_x - dx, sensor_y - dy));
            edge.insert((sensor_x - dx, sensor_y + dy));
            edge.insert((sensor_x + dx, sensor_y - dy));
            edge.insert((sensor_x + dx, sensor_y + dy));
        }

        // Take boundaries into account
        edge.retain(|&(x, y)| x >= 0 && x <= max && y >= 0 && y <= max);

        // Check if any of possible beacons is out of reach of other sensors
        for ((sensor_x, sensor_y), (beacon_x, beacon_y)) in sensors {
            let distance = (sensor_x - beacon_x).abs() + (sensor_y - beacon_y).abs();
            edge.retain(|&(x, y)| (x - sensor_x).abs() + (y - sensor_y).abs() > distance);
        }

        if !edge.is_empty() {
            let &(distress_x, distress_y) = edge.iter().next().unwrap();
            return distress_x as u64 * 4000000 + distress_y as u64;
        }
    }

    panic!("Distress beacon not found!");
}
