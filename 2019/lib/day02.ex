defmodule Aoc2019.Day02 do
  def run do
    test_program = parse_input("inputs/day_02_test.txt")
    test(test_program)

    program = parse_input("inputs/day_02.txt")
    program |> run_program(12, 2) |> IO.puts()
    program |> find_inputs(19_690_720) |> IO.puts()
  end

  defp test(test_program) do
    require ExUnit.Assertions
    import ExUnit.Assertions

    test_intcode = fn input, output ->
      assert IntcodeComputer.parse_program(input)
             |> IntcodeComputer.run()
             |> IntcodeComputer.print_memory() ==
               output
    end

    test_intcode.("1,0,0,0,99", "2,0,0,0,99")
    test_intcode.("2,3,0,3,99", "2,3,0,6,99")
    test_intcode.("2,4,4,5,99,0", "2,4,4,5,99,9801")
    test_intcode.("1,1,1,4,99,5,6,0,99", "30,1,1,4,2,5,6,0,99")

    assert IntcodeComputer.run(test_program)[0] == 3500
  end

  defp parse_input(file_name) do
    File.read!(file_name)
    |> String.trim()
    |> IntcodeComputer.parse_program()
  end

  defp run_program(program, noun, verb) do
    program
    |> Map.replace!(1, noun)
    |> Map.replace!(2, verb)
    |> IntcodeComputer.run()
    |> Map.get(0)
  end

  defp find_inputs(program, output) do
    {noun, verb} =
      Stream.flat_map(
        0..99,
        fn noun -> Stream.map(0..99, fn verb -> {noun, verb} end) end
      )
      |> Enum.find(fn {noun, verb} ->
        run_program(program, noun, verb) == output
      end)

    100 * noun + verb
  end
end
