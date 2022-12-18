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

    let cubes = read_file(&input_path);

    println!("{}", first_puzzle(&cubes));
    println!("{}", second_puzzle(&cubes));
}

type Cube = (usize, usize, usize);

fn read_file(path: &str) -> Vec<Cube> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let line = line
                .split(',')
                .map(|x| x.parse().unwrap())
                .collect::<Vec<_>>();
            (line[0], line[1], line[2])
        })
        .collect()
}

fn test(test_input_path: &str) {
    let cubes = read_file(test_input_path);
    assert_eq!(first_puzzle(&cubes), 64);
    assert_eq!(second_puzzle(&cubes), 58);
}

fn create_grid(cubes: &[Cube], empty: i8) -> Vec<Vec<Vec<i8>>> {
    let x_max = cubes.iter().map(|&(x, _, _)| x).max().unwrap();
    let y_max = cubes.iter().map(|&(_, y, _)| y).max().unwrap();
    let z_max = cubes.iter().map(|&(_, _, z)| z).max().unwrap();

    let mut grid = vec![vec![vec![empty; z_max + 1]; y_max + 1]; x_max + 1];

    // Mark cubes
    for &(x, y, z) in cubes {
        grid[x][y][z] = 1;
    }

    grid
}

fn mark_outside(grid: &mut Vec<Vec<Vec<i8>>>, x: usize, y: usize, z: usize) {
    // Skip lava droplets
    if grid[x][y][z] != 0 {
        return;
    }

    grid[x][y][z] = -1;

    // Mark neighbours
    if x > 0 {
        mark_outside(grid, x - 1, y, z);
    }
    if x + 1 < grid.len() {
        mark_outside(grid, x + 1, y, z);
    }
    if y > 0 {
        mark_outside(grid, x, y - 1, z);
    }
    if y + 1 < grid[x].len() {
        mark_outside(grid, x, y + 1, z);
    }
    if z > 0 {
        mark_outside(grid, x, y, z - 1);
    }
    if z + 1 < grid[x][y].len() {
        mark_outside(grid, x, y, z + 1);
    }
}

fn number_of_neighbours(grid: &Vec<Vec<Vec<i8>>>, x: usize, y: usize, z: usize) -> u32 {
    let mut neighbours = 0;

    if x > 0 && grid[x - 1][y][z] != -1 {
        neighbours += 1;
    }
    if x + 1 < grid.len() && grid[x + 1][y][z] != -1 {
        neighbours += 1;
    }
    if y > 0 && grid[x][y - 1][z] != -1 {
        neighbours += 1;
    }
    if y + 1 < grid[x].len() && grid[x][y + 1][z] != -1 {
        neighbours += 1;
    }
    if z > 0 && grid[x][y][z - 1] != -1 {
        neighbours += 1;
    }
    if z + 1 < grid[x][y].len() && grid[x][y][z + 1] != -1 {
        neighbours += 1;
    }

    neighbours
}

fn calculate_surface(grid: &Vec<Vec<Vec<i8>>>) -> u32 {
    let mut surface = 0;

    for x in 0..grid.len() {
        for y in 0..grid[x].len() {
            for z in 0..grid[x][y].len() {
                if grid[x][y][z] != 1 {
                    continue;
                }

                surface += 6 - number_of_neighbours(grid, x, y, z);
            }
        }
    }

    surface
}

fn first_puzzle(cubes: &[Cube]) -> u32 {
    let grid = create_grid(cubes, -1);
    calculate_surface(&grid)
}

fn second_puzzle(cubes: &[Cube]) -> u32 {
    let mut grid = create_grid(cubes, 0);

    // Change outside air values to -1
    for x in 0..grid.len() {
        for y in 0..grid[x].len() {
            for z in 0..grid[x][y].len() {
                let edge_x = x == 0 || x + 1 == grid.len();
                let edge_y = y == 0 || y + 1 == grid[x].len();
                let edge_z = z == 0 || z + 1 == grid[x][y].len();

                if edge_x || edge_y || edge_z && grid[x][y][z] == 0 {
                    mark_outside(&mut grid, x, y, z);
                }
            }
        }
    }

    calculate_surface(&grid)
}
