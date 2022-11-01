class test {
	
	public static void main(String[] args) {

		int[] arr = {76, 37, 23456, 678, 3457, 678, 2345, 4756, 346, 3254, 5467, 732345, 234, 41, 2, 1, 1, 523, 65, 2346, 3687, 23145, 1234, 5462, 65, 74, 648 };
		
		for (int i = 1; i < arr.length; i++) toHeap(i, arr);

		sort(arr);

		show(arr);
	}

	public static void toHeap(int limit, int[] arr) {
			
		for (int i = limit; arr[i] > arr[(i-1)/2]; i = (i-1)/2) {
			
			int tmp = arr[i];
			arr[i] = arr[(i-1)/2];
			arr[(i-1)/2] = tmp;
		}
	}

	public static void sort(int[] arr) {
		
		int firstToLast = arr.length;

		while (firstToLast > 0) {
			
			firstToLast--;

			int tmp = arr[0];
			arr[0] = arr[firstToLast];
			arr[firstToLast] = tmp;
			
			rebuild(firstToLast, arr);
		}
	}

	public static void rebuild(int firstToLast, int[] arr) {

		int i = 0;

		while (i < firstToLast/2) {
			
			int highest = i, left = 2 * i + 1, right = 2 * i + 2;

			if (left < firstToLast && arr[left] > arr[highest]) highest = left;
			if (right < firstToLast && arr[right] > arr[highest]) highest = right;
			
			if (arr[highest] > arr[i]) {

				int tmp = arr[i];
				arr[i] = arr[highest];
				arr[highest] = tmp;

				i = highest;

			} else break;
		}
	}

	public static void show(int[] arr) {
		
		for (int i = 0; i < arr.length - 1; i++) System.out.print(arr[i] + " ");

		System.out.println(arr[arr.length - 1]);
	}
}
