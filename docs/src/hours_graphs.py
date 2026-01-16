# /// script
# requires-python = ">=3.14"
# dependencies = [
#     "matplotlib>=3.10.8",
#     "pandas>=2.3.3",
# ]
# ///
import pandas as pd
import matplotlib.pyplot as plt

# Define the data
data = {
    "Name": ["Javier", "Piervito", "Matteo", "Emiliano", "Jorge", "Maxime", "Balsa"],
    "Sprint 1": [7, 5, 9, 9, 8, 6, 6],
    "Sprint 2": [13, 15, 15, 19, 14, 16, 10],
    "Sprint 3": [18, 22, 15, 13, 24, 22, 15],
    "Sprint 4": [11, 11, 11, 17, 13, 20, 15],
    "Sprint 5": [13, 13, 14, 15, 19, 15, 15],
    "Sprint 6": [10, 7, 7, 10, 10, 10, 8],
    "Sprint 7": [12, 16, 16, 12, 12, 16, 12],  # TODO: Review this
}

df = pd.DataFrame(data)

# Calculate totals
df["Total"] = df.iloc[:, 1:].sum(axis=1)

# Sprint totals
sprint_cols = [
    "Sprint 1",
    "Sprint 2",
    "Sprint 3",
    "Sprint 4",
    "Sprint 5",
    "Sprint 6",
    "Sprint 7",
]
team_totals = df[sprint_cols].sum()

# Graph 1: Total Hours Spent by Each Person
plt.figure(figsize=(10, 6))
plt.bar(df["Name"], df["Total"], color="skyblue", edgecolor="navy")
plt.title("Total Hours Spent by Each Person")
plt.xlabel("Team Member")
plt.ylabel("Total Hours")
plt.grid(axis="y", linestyle="--", alpha=0.7)
plt.tight_layout()
plt.savefig("total_hours_per_person.svg")
plt.close()

# Graph 2: Total Team Hours Per Sprint
plt.figure(figsize=(10, 6))
team_totals.plot(kind="bar", color="salmon", edgecolor="darkred")
plt.title("Total Team Hours Per Sprint")
plt.xlabel("Sprint")
plt.ylabel("Total Team Hours")
plt.grid(axis="y", linestyle="--", alpha=0.7)
plt.xticks(rotation=0)
plt.tight_layout()
plt.savefig("total_team_hours_per_sprint.svg")
plt.close()

# Graph 3: Cumulative Burn-up Chart
cumulative_hours = team_totals.cumsum()
plt.figure(figsize=(10, 6))
plt.plot(
    sprint_cols,
    cumulative_hours,
    marker="o",
    color="forestgreen",
    linewidth=2,
    label="Total Hours Done",
)
plt.fill_between(sprint_cols, cumulative_hours, color="forestgreen", alpha=0.1)
plt.title("Cumulative Team Burn-up (Total Hours Progress)")
plt.xlabel("Sprints")
plt.ylabel("Total Cumulative Hours")
plt.grid(True, linestyle="--", alpha=0.6)
for i, txt in enumerate(cumulative_hours):
    plt.annotate(
        int(txt),
        (sprint_cols[i], cumulative_hours[i]),
        textcoords="offset points",
        xytext=(0, 10),
        ha="center",
    )
plt.ylim(0, cumulative_hours.max() * 1.1)
plt.tight_layout()
plt.savefig("cumulative_burnup.svg")
plt.close()

print(df)
print("\nTeam Totals per Sprint:")
print(team_totals)
