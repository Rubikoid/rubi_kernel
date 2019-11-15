int_back = int
class rint(int_back):
    def __str__(self):
        return 'f'
    def __repr__(self):
        return f"f"
int = rint
