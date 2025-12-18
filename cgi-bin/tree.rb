def print_fancy_tree(height)
  return if height < 3
  num_layers = [height / 3, 3].max
  layer_height = height / num_layers
  current_width = 1
  total_width = height * 2 + 1
  center = total_width / 2
  num_layers.times do |layer|
    rows = layer_height + (layer == num_layers - 1 ? height % num_layers : 0)

    rows.times do |i|
      stars = current_width + (i * 2)
      spaces = center - (stars / 2)
      line = ""
      stars.times do |s|
        if s == 0 || s == stars - 1
          line += "*"
        elsif rand < 0.15  # 15% chance of ornament
          line += ["o", "O", "@", "+"].sample
        else
          line += "*"
        end
      end
      puts " " * spaces + line
    end
    current_width += 2
  end
  trunk_width = [height / 4, 3].max
  trunk_height = [height / 3, 2].max
  trunk_spaces = center - (trunk_width / 2)
  trunk_height.times do
    trunk_line = ""
    trunk_width.times do |i|
      trunk_line += "|" if i == 0 || i == trunk_width - 1
      trunk_line += "#" if i > 0 && i < trunk_width - 1
    end
    puts " " * trunk_spaces + trunk_line
  end
  ground_width = trunk_width + 4
  ground_spaces = center - (ground_width / 2)
  puts " " * ground_spaces + "~" * ground_width
end

def print_custom_tree(height, ornaments: true, snow: false, star: true)
  return if height < 3
  total_width = height * 2 + 1
  center = total_width / 2
  if star
    puts " " * center + "★"
  end
  num_layers = [height / 3, 3].max
  layer_height = height / num_layers
  current_width = 1
  num_layers.times do |layer|
    rows = layer_height + (layer == num_layers - 1 ? height % num_layers : 0)
    rows.times do |i|
      stars = current_width + (i * 2)
      spaces = center - (stars / 2)
      line = ""
      stars.times do |s|
        char = "*"
        if s == 0 || s == stars - 1
          char = "*"
        elsif snow && layer < 2 && i < 2 && rand < 0.3
          char = "."
        elsif ornaments && rand < 0.12
          char = ["o", "O", "@", "+", "♦", "♥"].sample
        end
        line += char
      end
      puts " " * spaces + line
    end
    current_width += 2
  end
  trunk_width = [height / 4, 3].max
  trunk_height = [height / 3, 2].max
  trunk_spaces = center - (trunk_width / 2)
  trunk_height.times do |row|
    trunk_line = ""
    trunk_width.times do |col|
      if col == 0 || col == trunk_width - 1
        trunk_line += "|"
      else
        trunk_line += (row + col) % 2 == 0 ? "#" : "█"
      end
    end
    puts " " * trunk_spaces + trunk_line
  end
  ground_width = trunk_width + 6
  ground_spaces = center - (ground_width / 2)
  ground = ""
  ground_width.times do |i|
    ground += ["~", "^", "v", "~"].sample
  end
  puts " " * ground_spaces + ground
end

puts "<p>"
height = rand(10..30)
if rand < 0.5
    print_fancy_tree(height)
else
    print_custom_tree(height, ornaments: [true, false].sample, snow: [true, false].sample, star: [true, false].sample)
end
puts "</p>"