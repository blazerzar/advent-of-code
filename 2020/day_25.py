def get_loop_size(key: int) -> int:
    """Return number of loops needed to transform 7 into given key"""
    value = 1
    subject_number = 7
    loops = 0

    while value != key:
        loops += 1
        value *= subject_number
        value %= 20201227
    return loops


def get_encryption_key(public: int, loops: int) -> int:
    """Calculate encryption key from the public key
    with given number of loops
    """
    value = 1

    for _ in range(loops):
        value *= public
        value %= 20201227
    return value


def test():
    card_public = 5764801
    door_public = 17807724
    card_loops = get_loop_size(card_public)
    door_loops = get_loop_size(door_public)
    assert card_loops == 8
    assert door_loops == 11
    key_from_card = get_encryption_key(door_public, card_loops)
    key_from_door = get_encryption_key(card_public, door_loops)
    assert key_from_card == key_from_door and key_from_card == 14897079


def main():
    test()
    card_public = 11562782
    door_public = 18108497
    print(get_encryption_key(door_public, get_loop_size(card_public)))


if __name__ == "__main__":
    main()
