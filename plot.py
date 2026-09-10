#!/usr/bin/env python3

import re
import sys
import matplotlib.pyplot as plt
from collections import defaultdict


def parse_file(filename):
    # size -> list of (fail_rate, throughput)
    data = defaultdict(list)

    with open(filename, "r") as f:
        text = f.read()

    # Each benchmark has:
    # Benchmark <size> Byte Allocation Maximum
    # ...
    # Fail Rate: <rate>
    # Throughput: <throughput>

    pattern = re.compile(
        r"Benchmark\s+(\d+)\s+Byte Allocation Maximum"
        r".*?"
        r"Fail Rate:\s*([0-9.eE+-]+)"
        r"\s+Throughput:\s*([0-9.eE+-]+)",
        re.DOTALL
    )

    for match in pattern.finditer(text):
        size = int(match.group(1))
        fail_rate = float(match.group(2))
        throughput = float(match.group(3))

        data[size].append((fail_rate, throughput))

    # Average repeated runs
    averages = {}

    for size, results in data.items():
        fail_rates = [x[0] for x in results]
        throughputs = [x[1] for x in results]

        averages[size] = {
            "fail_rate": sum(fail_rates) / len(fail_rates),
            "throughput": sum(throughputs) / len(throughputs),
            "runs": len(results),
        }

    return averages


def plot_results(data1, data2, name1, name2):
    sizes1 = sorted(data1)
    sizes2 = sorted(data2)

    fail1 = [data1[x]["fail_rate"] for x in sizes1]
    fail2 = [data2[x]["fail_rate"] for x in sizes2]

    throughput1 = [data1[x]["throughput"] for x in sizes1]
    throughput2 = [data2[x]["throughput"] for x in sizes2]

    # --------------------
    # Fail Rate
    # --------------------
    plt.figure()

    plt.plot(
        sizes1,
        fail1,
        marker="o",
        label=name1
    )

    plt.plot(
        sizes2,
        fail2,
        marker="o",
        label=name2
    )

    plt.xlabel("Maximum Allocation Size (bytes)")
    plt.ylabel("Average Fail Rate")
    plt.title("Allocation Fail Rate")
    plt.xscale("log", base=2)
    plt.grid(True)
    plt.legend()

    plt.tight_layout()
    plt.savefig("plot-fail_rate.png", dpi=300)
    plt.show()

    # --------------------
    # Throughput
    # --------------------
    plt.figure()

    plt.plot(
        sizes1,
        throughput1,
        marker="o",
        label=name1
    )

    plt.plot(
        sizes2,
        throughput2,
        marker="o",
        label=name2
    )

    plt.xlabel("Maximum Allocation Size (bytes)")
    plt.ylabel("Average Throughput (allocations/sec)")
    plt.title("Allocation Throughput")
    plt.xscale("log", base=2)
    plt.grid(True)
    plt.legend()

    plt.tight_layout()
    plt.savefig("plot-throughput.png", dpi=300)
    plt.show()


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <allocator1.txt> <allocator2.txt>")
        sys.exit(1)

    file1 = sys.argv[1]
    file2 = sys.argv[2]

    data1 = parse_file(file1)
    data2 = parse_file(file2)

    # Use filenames as legend labels
    name1 = file1.rsplit("/", 1)[-1]
    name2 = file2.rsplit("/", 1)[-1]

    print(f"{name1}:")
    for size in sorted(data1):
        d = data1[size]
        print(
            f"  {size:5d} bytes: "
            f"fail={d['fail_rate']:.6f}, "
            f"throughput={d['throughput']:.2f} "
            f"({d['runs']} runs)"
        )

    print(f"\n{name2}:")
    for size in sorted(data2):
        d = data2[size]
        print(
            f"  {size:5d} bytes: "
            f"fail={d['fail_rate']:.6f}, "
            f"throughput={d['throughput']:.2f} "
            f"({d['runs']} runs)"
        )

    plot_results(data1, data2, name1, name2)


if __name__ == "__main__":
    main()