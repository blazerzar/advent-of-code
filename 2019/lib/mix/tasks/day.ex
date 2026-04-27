defmodule Mix.Tasks.Day do
  use Mix.Task

  @shortdoc "Run a specific AoC day"
  def run([day]) do
    module = Module.concat(Aoc2019, "Day#{String.pad_leading(day, 2, "0")}")
    apply(module, :run, [])
  end
end
