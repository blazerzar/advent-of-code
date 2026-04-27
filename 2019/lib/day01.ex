defmodule Aoc2019.Day01 do
  def run do
    test()

    masses = parse_input("inputs/day_01.txt")

    Enum.map(masses, &module_fuel_required/1) |> Enum.sum() |> IO.puts()
    Enum.map(masses, &module_fuel_required_with_fuel/1) |> Enum.sum() |> IO.puts()
  end

  defp test() do
    require ExUnit.Assertions
    import ExUnit.Assertions

    assert module_fuel_required(12) == 2
    assert module_fuel_required(14) == 2
    assert module_fuel_required(1969) == 654
    assert module_fuel_required(100_756) == 33583

    assert module_fuel_required_with_fuel(14) == 2
    assert module_fuel_required_with_fuel(1969) == 966
    assert module_fuel_required_with_fuel(100_756) == 50346
  end

  defp parse_input(file_name) do
    File.read!(file_name)
    |> String.trim()
    |> String.split("\n")
    |> Enum.map(&String.to_integer/1)
  end

  defp module_fuel_required(mass), do: div(mass, 3) - 2

  defp module_fuel_required_with_fuel(mass) when mass < 9, do: 0

  defp module_fuel_required_with_fuel(mass) do
    fuel = module_fuel_required(mass)
    fuel + module_fuel_required_with_fuel(fuel)
  end
end
