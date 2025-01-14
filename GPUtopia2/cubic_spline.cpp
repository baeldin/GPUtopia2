#include "cubic_spline.h"

/* BASIC SOLVER FOR SYSTEMS OF LINEAR EQUATIONS
 *
 * This does NOT account for linearly dependent rows in the
 * coefficient matrix!!!
 *
 * This code is also not optimized, do not use for systems of many
 * equations.
 *
 * Solves linear equations if matrix can be diagonlalized and
 * the solution is unique. This is fine for cubic splines because
 * the coefficients for those are always well defined and unique.
 *
 * This code comes without guarantees. Make sure to double check the results
 * when using it!!
 */

std::vector<double> solve_linear_eqs(std::vector<double> M, std::vector<double> b)
{
	int n = b.size();
	for (int ii = 0; ii < n; ii++)
	{
		for (int jj = 0; jj < n; jj++)
		{
			if (jj != ii) // do not use the row to eliminate itself, DUH!
			{
				if (M[(1 + n) * ii] == 0)
				{
					// if there is a zero, do not use this row for the next step
					// find a row below that does not have a zero in this column
					int biggest_val_idx = 0;
					double abs_val = 0;
					for (int kk = ii + 1; kk < n; kk++)
					{
						biggest_val_idx = (M[n * kk + ii] > abs_val) ? kk : biggest_val_idx;
					}
					// swap row ii with row biggest_index_val
					for (int kk = 0; kk < n; kk++)
					{
						std::swap(M[n * ii + kk], M[n * biggest_val_idx + kk]);
					}
					std::swap(b[ii], b[biggest_val_idx]);
				}
				// once we're sure that M[n*ii+ii] is not zero, use it for eliminating
				double fac = M[n * jj + ii] / M[n * ii + ii];
				b[jj] = b[jj] - fac * b[ii];
				for (int kk = 0; kk < n; kk++)
				{
					M[n * jj + kk] = M[n * jj + kk] - fac * M[n * ii + kk];
				}
			}
		}
	}
	for (int ii = 0; ii < n; ii++)
	{
		b[ii] = b[ii] / M[n * ii + ii];
		M[n * ii + ii] = M[n * ii + ii] / M[n * ii + ii];
	}
	return b;
}

std::vector<double> calculate_spline_coefficients(std::vector<float> x, std::vector<float> y)
{
	// slopes, limit to 0 for extreme points
	float s1 = (y[1] >= y[0] && y[1] >= y[2]) ? 0 : (y[2] - y[0]) / (x[2] - x[0]);
	float s2 = (y[2] >= y[1] && y[2] >= y[3]) ? 0 : (y[3] - y[1]) / (x[3] - x[1]);
	//cout << "Slopes: " << s1 << ", " << s2 << "\n";
	std::vector<double> M = {
			pow(x[1], 3), pow(x[1], 2), x[1], 1,
		3 * pow(x[1], 2),     2 * x[1],    1, 0,
			pow(x[2], 3), pow(x[2], 2), x[2], 1,
		3 * pow(x[2], 2),     2 * x[2],    1, 0 };
	std::vector<double> b = { y[1], s1, y[2], s2 };
	std::vector<double> coeffs = solve_linear_eqs(M, b);
	return coeffs;
}


color splined_color(std::vector<float> x, std::vector<color> colors, float index)
{
	//cout << "Received 4 indices and 4 colors:\n";
	//for (int ii = 0; ii < 4; ii++)
	//{
	//	cout << "index: " << x[ii] << " color: " << colors[ii].r << " " << colors[ii].g << " " << colors[ii].b << "\n";
	//}
	std::vector<double> coeffsRed = calculate_spline_coefficients(
		x, { colors[0].r, colors[1].r, colors[2].r, colors[3].r });
	std::vector<double> coeffsGreen = calculate_spline_coefficients(
		x, { colors[0].g, colors[1].g, colors[2].g, colors[3].g });
	std::vector<double> coeffsBlue = calculate_spline_coefficients(
		x, { colors[0].b, colors[1].b, colors[2].b, colors[3].b });
	//cout << coeffsBlue[0] << "\n";
	return color(
		std::min<float>(1.f, (std::max<float>(0.f, coeffsRed[0] * pow(index, 3) + coeffsRed[1] * pow(index, 2) + coeffsRed[2] * index + coeffsRed[3]))),
		std::min<float>(1.f, (std::max<float>(0.f, coeffsGreen[0] * pow(index, 3) + coeffsGreen[1] * pow(index, 2) + coeffsGreen[2] * index + coeffsGreen[3]))),
		std::min<float>(1.f, (std::max<float>(0.f, coeffsBlue[0] * pow(index, 3) + coeffsBlue[1] * pow(index, 2) + coeffsBlue[2] * index + coeffsBlue[3]))));
}