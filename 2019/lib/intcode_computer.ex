defmodule IntcodeComputer do
  def run(program, ip \\ 0) do
    case program[ip] do
      1 ->
        Map.replace!(
          program,
          program[ip + 3],
          program[program[ip + 1]] + program[program[ip + 2]]
        )
        |> run(ip + 4)

      2 ->
        Map.replace!(
          program,
          program[ip + 3],
          program[program[ip + 1]] * program[program[ip + 2]]
        )
        |> run(ip + 4)

      99 ->
        program
    end
  end

  def parse_program(program) do
    program
    |> String.split(",")
    |> Enum.with_index(fn val, i -> {i, String.to_integer(val)} end)
    |> Map.new()
  end

  def print_memory(memory) do
    memory |> Enum.sort() |> Enum.map_join(",", fn {_i, val} -> val end)
  end
end
