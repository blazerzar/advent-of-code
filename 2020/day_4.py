import re
import os


def valid_passports(passports: list, check_values: bool) -> int:
    """Return number of valid passports"""
    keys = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"}
    count = 0
    for passport in passports:
        if not keys.difference(passport.keys()):
            if check_values:
                for key in keys:
                    if not valid_attribute(key, passport[key]):
                        break
                else:
                    count += 1
            else:
                count += 1
    return count


def valid_attribute(attr_key: str, attr_value: str) -> bool:
    """Check if given attribute has correct value"""
    if attr_key == "byr":
        return (attr_value.isnumeric() and len(attr_value) == 4
                and 1920 <= int(attr_value) <= 2002)
    elif attr_key == "iyr":
        return (attr_value.isnumeric() and len(attr_value) == 4
                and 2010 <= int(attr_value) <= 2020)
    elif attr_key == "eyr":
        return (attr_value.isnumeric() and len(attr_value) == 4
                and 2020 <= int(attr_value) <= 2030)
    elif attr_key == "hgt":
        match = re.fullmatch(r"(\d+)(cm|in)", attr_value)
        if match:
            if match.group(2) == "cm":
                return 150 <= int(match.group(1)) <= 193
            elif match.group(2) == "in":
                return 59 <= int(match.group(1)) <= 76
        return False
    elif attr_key == "hcl":
        return re.fullmatch(r"#[0-9a-f]{6}", attr_value) is not None
    elif attr_key == "ecl":
        return re.fullmatch(r"amb|blu|brn|gry|grn|hzl|oth",
                            attr_value) is not None
    elif attr_key == "pid":
        return re.fullmatch(r"\d{9}", attr_value) is not None
    return True


def read_file(file_name: str) -> list:
    passports = []
    index = 0
    with open(os.path.join(os.path.dirname(__file__), file_name), "r") as f:
        for line in f:
            if not line.strip():
                index += 1
            elif len(passports) > index:
                passports[index].update({key: value for key, value in [
                    item.split(":") for item in line.split()]})
            else:
                passports.append({key: value for key, value in [
                    item.split(":") for item in line.split()]})
    return passports


def test():
    passports = read_file("inputs/input4_test.txt")
    assert valid_passports(passports, False) == 2
    assert valid_attribute("byr", "1967") is True
    assert valid_attribute("byr", "2002") is True
    assert valid_attribute("byr", "2003") is False
    assert valid_attribute("byr", "1899") is False
    assert valid_attribute("byr", "001970") is False
    assert valid_attribute("iyr", "2012") is True
    assert valid_attribute("eyr", "2015") is False
    assert valid_attribute("eyr", "2022") is True
    assert valid_attribute("hgt", "60in") is True
    assert valid_attribute("hgt", "190cm") is True
    assert valid_attribute("hgt", "60in") is True
    assert valid_attribute("hgt", "190cm") is True
    assert valid_attribute("hgt", "190in") is False
    assert valid_attribute("hgt", "190") is False
    assert valid_attribute("hcl", "#123abc") is True
    assert valid_attribute("hcl", "#123abc") is True
    assert valid_attribute("hcl", "#123abz") is False
    assert valid_attribute("hcl", "123abc") is False
    assert valid_attribute("ecl", "brn") is True
    assert valid_attribute("ecl", "wat") is False
    assert valid_attribute("pid", "000125432") is True
    assert valid_attribute("pid", "000000001") is True
    assert valid_attribute("pid", "0123456789") is False


def main():
    test()
    passports = read_file("inputs/input4.txt")
    print(valid_passports(passports, False))
    print(valid_passports(passports, True))


if __name__ == "__main__":
    main()
