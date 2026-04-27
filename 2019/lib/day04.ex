defmodule Aoc2019.Day04 do
  def run do
    test()

    range = parse_input("inputs/day_04.txt")
    valid_passwords(range, false) |> IO.puts()
    valid_passwords(range, true) |> IO.puts()
  end

  defp test() do
    require ExUnit.Assertions
    import ExUnit.Assertions

    assert meets_password_criteria?(111_111, false) == true
    assert meets_password_criteria?(223_450, false) == false
    assert meets_password_criteria?(123_789, false) == false
    assert meets_password_criteria?(112_233, true) == true
    assert meets_password_criteria?(123_444, true) == false
    assert meets_password_criteria?(111_122, true) == true
  end

  defp parse_input(file_name) do
    File.read!(file_name)
    |> String.trim()
    |> String.split("-")
    |> Enum.map(fn s ->
      {n, _} = Integer.parse(s)
      n
    end)
  end

  defp double_digits?([a, b, c, d, e, f], strict) do
    if strict do
      (a == b and b != c) or
        (a != b and b == c and c != d) or
        (b != c and c == d and d != e) or
        (c != d and d == e and e != f) or
        (d != e and e == f)
    else
      a == b or b == c or c == d or d == e or e == f
    end
  end

  defp digits_increase?([a, b, c, d, e, f]) do
    a <= b and b <= c and c <= d and d <= e and e <= f
  end

  defp meets_password_criteria?(password, strict) do
    password = Integer.digits(password)
    double_digits?(password, strict) and digits_increase?(password)
  end

  defp valid_passwords([first, last], strict) do
    Range.new(first, last)
    |> Enum.count(fn password ->
      meets_password_criteria?(password, strict)
    end)
  end
end
