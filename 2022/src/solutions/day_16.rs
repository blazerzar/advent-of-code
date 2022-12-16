use regex::Regex;
use std::collections::{HashMap, LinkedList};
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

    let valves = read_file(&input_path);
    println!("{}", first_puzzle(&valves));

    // Run in release mode for faster calculation
    println!("{}", second_puzzle(&valves));
}

struct Valve {
    id: usize,
    label: String,
    flow_rate: u32,
    tunnels: Vec<usize>,
}

fn read_file(path: &str) -> Vec<Valve> {
    let re = Regex::new(r"^Valve ([A-Z]{2}).*=(\d+); .* valves? ([A-Z, ]+)").unwrap();
    let mut number_of_valves: usize = 0;
    let mut valve_ids = HashMap::new();

    // Parse input and give each valve an id
    let valves = fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let cap = re.captures(line).unwrap();
            let valve = (
                cap[1].to_string(),
                cap[2].parse().unwrap(),
                cap[3].split(", ").map(String::from).collect::<Vec<_>>(),
            );
            valve_ids.insert(valve.0.clone(), number_of_valves);
            number_of_valves += 1;
            valve
        })
        .collect::<Vec<(String, u32, Vec<String>)>>();

    // Create objects for valves using ids instead of strings
    valves
        .iter()
        .map(|(valve, flow_rate, tunnels)| Valve {
            id: *valve_ids.get(valve).unwrap(),
            label: valve.clone(),
            flow_rate: *flow_rate,
            tunnels: tunnels
                .iter()
                .map(|valve_str| *valve_ids.get(valve_str).unwrap())
                .collect(),
        })
        .collect()
}

fn test(test_input_path: &str) {
    let valves = read_file(test_input_path);
    assert_eq!(first_puzzle(&valves), 1651);
    assert_eq!(second_puzzle(&valves), 1707);
}

fn find_valve_distances(valves: &[Valve]) -> Vec<Vec<u32>> {
    let mut distances = vec![vec![u32::MAX; valves.len()]; valves.len()];

    // Distances to neighbours are 1
    for valve in valves {
        for neighbour in &valve.tunnels {
            distances[valve.id][*neighbour] = 1;
        }
    }

    // To find shortest distances we need as many iterations as there are valves
    for _ in 0..valves.len() {
        for valve in valves {
            for valve_update in valves {
                if valve.id != valve_update.id {
                    // Update distances by looking at neighbours' distances
                    let current_distance = distances[valve.id][valve_update.id];
                    let neighbours_min = valve
                        .tunnels
                        .iter()
                        .map(|&neighbour| distances[neighbour][valve_update.id])
                        .min()
                        .unwrap();

                    // Only use neighbours' distance, if it has already been updated
                    distances[valve.id][valve_update.id] =
                        current_distance.min(if neighbours_min < u32::MAX {
                            neighbours_min + 1
                        } else {
                            neighbours_min
                        });
                }
            }
        }
    }

    distances
}

fn first_puzzle(valves: &[Valve]) -> u32 {
    // Find distance between each pair of valves
    let distances = find_valve_distances(valves);

    let mut to_open: u64 = 0;
    let mut start = 0;
    for valve in valves {
        to_open |= u64::from(valve.flow_rate > 0) << valve.id;
        if valve.label == "AA" {
            start = valve.id;
        }
    }

    let mut q = LinkedList::new();
    q.push_back((start, 1, 0, to_open));

    let mut best_pressure = 0;

    while let Some((valve, minute, pressure, to_open)) = q.pop_back() {
        best_pressure = best_pressure.max(pressure);

        // Add new states
        let new_states = valves
            .iter()
            // Find valves which can be opened
            .filter(|valve| to_open & (1 << valve.id) > 0)
            // Move to valve, mark it as opened and add its pressure
            .map(|valve_to_open| {
                let distance = distances[valve][valve_to_open.id];

                // Do not calculate pressure, if time ran out (prevents overflow)
                let mut pressure = pressure;
                if minute + distance <= 30 {
                    pressure += valve_to_open.flow_rate * (30 - minute - distance);
                }

                (
                    valve_to_open.id,
                    minute + distance + 1,
                    pressure,
                    to_open & !(1 << valve_to_open.id),
                )
            })
            // Throw away moves which go over the time limit
            .filter(|&(_, minute, _, _)| minute <= 30);

        q.extend(new_states);
    }

    best_pressure
}

fn second_puzzle(valves: &[Valve]) -> u32 {
    // Find distance between each pair of valves
    let distances = find_valve_distances(valves);

    let mut to_open: u64 = 0;
    let mut start = 0;
    for valve in valves {
        to_open |= u64::from(valve.flow_rate > 0) << valve.id;
        if valve.label == "AA" {
            start = valve.id;
        }
    }

    let mut q = LinkedList::new();
    q.push_back((start, start, 1, 1, 0, to_open));

    let mut best_pressure = 0;

    while let Some((me, elephant, minute_me, minute_elephant, pressure, to_open)) = q.pop_back() {
        best_pressure = best_pressure.max(pressure);

        // Find all valves which I and elephant can open before time runs out
        let valves_to_open = valves
            .iter()
            .filter(|valve| to_open & (1 << valve.id) > 0)
            .collect::<Vec<_>>();

        // Check if all valves together can improve the best score so far
        let combined_flow_rate: u32 = valves_to_open.iter().map(|valve| valve.flow_rate).sum();
        if pressure + combined_flow_rate * (26 - minute_me.min(minute_elephant)) < best_pressure {
            continue;
        }

        // Elephant is always ahead of the player. This cuts a lot of states
        // to check and it works because game is symmetric. We can change
        // player and elephant around and get the same result.
        if minute_me <= minute_elephant {
            // Add states where I open one valve
            let new_states = valves_to_open
                .iter()
                .map(|valve_to_open| {
                    let distance = distances[me][valve_to_open.id];

                    let mut pressure = pressure;
                    if minute_me + distance <= 26 {
                        pressure += valve_to_open.flow_rate * (26 - minute_me - distance);
                    }

                    (
                        valve_to_open.id,
                        elephant,
                        minute_me + distance + 1,
                        minute_elephant,
                        pressure,
                        to_open & !(1 << valve_to_open.id),
                    )
                })
                .filter(|&(_, _, minute_me, _, _, _)| minute_me <= 26);

            q.extend(new_states);
        } else {
            // Add states where elephant opens one valve
            let new_states = valves_to_open
                .iter()
                .map(|valve_to_open| {
                    let distance = distances[elephant][valve_to_open.id];

                    let mut pressure = pressure;
                    if minute_elephant + distance <= 26 {
                        pressure += valve_to_open.flow_rate * (26 - minute_elephant - distance);
                    }

                    (
                        me,
                        valve_to_open.id,
                        minute_me,
                        minute_elephant + distance + 1,
                        pressure,
                        to_open & !(1 << valve_to_open.id),
                    )
                })
                .filter(|&(_, _, _, minute_elephant, _, _)| minute_elephant <= 26);

            q.extend(new_states);
        }
    }

    best_pressure
}
