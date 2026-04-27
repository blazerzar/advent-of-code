defmodule Aoc2019.Day03 do
  def run do
    test_wires_1 = parse_input("inputs/day_03_test_1.txt")
    test_wires_2 = parse_input("inputs/day_03_test_2.txt")
    test_wires_3 = parse_input("inputs/day_03_test_3.txt")
    test(test_wires_1, test_wires_2, test_wires_3)

    wires = parse_input("inputs/day_03.txt")
    closest_intersection(wires) |> IO.puts()
    fewest_steps_intersection(wires) |> IO.puts()
  end

  defp test(test_wires_1, test_wires_2, test_wires_3) do
    require ExUnit.Assertions
    import ExUnit.Assertions

    assert closest_intersection(test_wires_1) == 6
    assert closest_intersection(test_wires_2) == 159
    assert closest_intersection(test_wires_3) == 135
    assert fewest_steps_intersection(test_wires_1) == 30
    assert fewest_steps_intersection(test_wires_2) == 610
    assert fewest_steps_intersection(test_wires_3) == 410
  end

  defp parse_input(file_name) do
    [first_wire, second_wire] =
      File.read!(file_name)
      |> String.trim()
      |> String.split()

    {parse_wire_path(first_wire), parse_wire_path(second_wire)}
  end

  defp parse_wire_path(wire_path) do
    String.split(wire_path, ",")
    |> Enum.map(fn move ->
      {direction, length} = String.split_at(move, 1)

      direction =
        case direction do
          "U" -> :up
          "R" -> :right
          "D" -> :down
          "L" -> :left
        end

      length = String.to_integer(length)

      {direction, length}
    end)
  end

  # Returns a list of turns from a wire path, where each turn is a tuple
  # `{x, y, distance_from_origin}` representing the coordinates of the turn
  # and the cumulative wire distance from the origin to that point. For
  # efficiency turns are reversed.
  defp path_turns(wire_path, turns \\ [{0, 0, 0}])
  defp path_turns([], turns), do: turns

  defp path_turns([{direction, length} | rest], turns) do
    {x, y, steps} = List.first(turns)
    new_steps = steps + length

    turn =
      case direction do
        :up -> {x, y + length, new_steps}
        :down -> {x, y - length, new_steps}
        :right -> {x + length, y, new_steps}
        :left -> {x - length, y, new_steps}
      end

    path_turns(rest, [turn | turns])
  end

  defp horizontal?([{_, y1, _}, {_, y2, _}]), do: y1 == y2

  defp check_intersection(
         [{hx1, hy, _}, {hx2, _, steps1}],
         [{vx, vy1, _}, {_, vy2, steps2}]
       ) do
    {left, right} = Enum.min_max([hx1, hx2])
    {bottom, top} = Enum.min_max([vy1, vy2])

    if left <= vx and vx <= right and bottom <= hy and hy <= top do
      length = steps1 + abs(hx2 - vx) + steps2 + abs(vy2 - hy)
      {:ok, {vx, hy, length}}
    else
      :none
    end
  end

  defp intersection(line1, line2) do
    cond do
      horizontal?(line1) and not horizontal?(line2) ->
        check_intersection(line1, line2)

      horizontal?(line2) and not horizontal?(line1) ->
        check_intersection(line2, line1)

      true ->
        :none
    end
  end

  defp intersections({first_wire, second_wire}) do
    first_turns = path_turns(first_wire)
    second_turns = path_turns(second_wire)
    first_lines = Enum.chunk_every(first_turns, 2, 1, :discard)
    second_lines = Enum.chunk_every(second_turns, 2, 1, :discard)

    for line1 <- first_lines, line2 <- second_lines do
      intersection(line1, line2)
    end
    |> Enum.filter(fn intersection ->
      case intersection do
        {:ok, {0, 0, _}} -> false
        {:ok, _} -> true
        _ -> false
      end
    end)
  end

  defp closest_intersection(wires) do
    intersections(wires)
    |> Enum.map(fn {_, {x, y, _}} -> abs(x) + abs(y) end)
    |> Enum.min()
  end

  defp fewest_steps_intersection(wires) do
    intersections(wires)
    |> Enum.map(fn {_, {_, _, steps}} -> steps end)
    |> Enum.min()
  end
end
