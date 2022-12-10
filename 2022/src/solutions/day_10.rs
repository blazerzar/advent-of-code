use std::fs;
use std::path::PathBuf;

enum Instruction {
    Noop,
    Addx(i32),
}

struct Cpu {
    cycle: i32,
    register: i32,
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

    let program = read_file(&input_path);
    println!("{}", first_puzzle(&program));
    println!("{}", second_puzzle(&program));
}

fn read_file(path: &str) -> Vec<Instruction> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let line = line.split_whitespace().collect::<Vec<_>>();
            match line[0] {
                "noop" => Instruction::Noop,
                _ => Instruction::Addx(line[1].parse().unwrap()),
            }
        })
        .collect()
}

fn test(test_input_path: &str) {
    let program = read_file(test_input_path);
    assert!(first_puzzle(&program) == 13140);

    let test_output = vec![
        "##..##..##..##..##..##..##..##..##..##..",
        "###...###...###...###...###...###...###.",
        "####....####....####....####....####....",
        "#####.....#####.....#####.....#####.....",
        "######......######......######......####",
        "#######.......#######.......#######.....",
    ]
    .join("\n");
    assert!(second_puzzle(&program) == test_output);
}

fn update_cpu(cpu: &mut Cpu, signal_strengths: &mut i32) {
    cpu.cycle += 1;
    if (cpu.cycle - 20) % 40 == 0 {
        *signal_strengths += cpu.cycle * cpu.register;
    }
}

fn first_puzzle(program: &[Instruction]) -> i32 {
    let mut cpu = Cpu {
        cycle: 1,
        register: 1,
    };
    let mut signal_strengths = 0;

    for instruction in program {
        match instruction {
            Instruction::Noop => {
                update_cpu(&mut cpu, &mut signal_strengths);
            }
            Instruction::Addx(value) => {
                update_cpu(&mut cpu, &mut signal_strengths);
                cpu.register += value;
                update_cpu(&mut cpu, &mut signal_strengths);
            }
        };
    }

    signal_strengths
}

fn draw_pixel(cpu: &mut Cpu, pixels: &mut String) {
    if ((cpu.cycle - 1) % 40 - cpu.register).abs() <= 1 {
        pixels.push('#');
    } else {
        pixels.push('.');
    };

    if cpu.cycle % 40 == 0 && cpu.cycle < 240 {
        pixels.push('\n');
    }
    cpu.cycle += 1;
}

fn second_puzzle(program: &[Instruction]) -> String {
    let mut cpu = Cpu {
        cycle: 1,
        register: 1,
    };
    let mut pixels = String::new();

    for instruction in program {
        match instruction {
            Instruction::Noop => {
                draw_pixel(&mut cpu, &mut pixels);
            }
            Instruction::Addx(value) => {
                draw_pixel(&mut cpu, &mut pixels);
                draw_pixel(&mut cpu, &mut pixels);
                cpu.register += value;
            }
        };
    }

    pixels
}
