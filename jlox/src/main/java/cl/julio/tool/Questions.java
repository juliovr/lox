package cl.julio.tool;

public class Questions {

    /**
     * There are N people who want to join a party. Some people will only go if at least L other people
     * will go with them. Additionally, they don't want to go with more than H other people, due to various
     * concerns.
     * Find the maximum number of people that can go to the party house at a time.
     *
     * Input:
     * 6
     * 1 2
     * 0 2
     * 1 4
     * 0 3
     * 0 1
     * 3 4
     *
     */
    public static void main(String[] args) {
        int n = 6;
        int[][] requirements = {
                { 1, 2 },
                { 0, 2 },
                { 1, 4 },
                { 0, 3 },
                { 0, 1 },
                { 3, 4 },
        };

        Questions q = new Questions();
        int result = q.solve(n, requirements);
        System.out.println(result);
    }

    private int solve(int n, int[][] requirements) {
        boolean[] people = new boolean[n];
        getMax(people, requirements, 0);

        return max;
    }

    int max = 0;

    private void getMax(boolean[] people, int[][] requirements, int i) {
        if (i >= people.length) return;

        people[i] = !people[i];
        int count = count(people);
        if (check(people, requirements, count)) {
            max = Math.max(max, count(people));
        }

        getMax(people, requirements, i + 1);
        people[i] = !people[i];
    }

    private boolean check(boolean[] people, int[][] requirements, int totalYes) {
        int totalYesWithoutHerself = totalYes - 1;
        for (int i = 0; i < requirements.length; i++) {
            if (people[i]) {
                int low = requirements[i][0];
                int high = requirements[i][1];

                if (totalYesWithoutHerself < low || totalYesWithoutHerself > high) {
                    return false;
                }
            }
        }

        return true;
    }

    private int count(boolean[] people) {
        int total = 0;
        for (boolean person : people) {
            if (person) {
                total++;
            }
        }

        return total;
    }

}
