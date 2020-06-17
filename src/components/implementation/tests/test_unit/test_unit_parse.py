test_failure = False;
with open("test_unit.txt", "r") as f:
    for line in f:
        l = line.strip()
        if("FAIL" in l):
            print(l)
            test_failure = True
if(test_failure == True):
    print("Tests Failed")
    exit(1)
print("All Tests Passed")
exit(0)

