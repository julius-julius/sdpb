#include <vector>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include "../../../sdp_solve.hxx"
#include "../../../dynamical_solve.hxx"
#include "../../../sdp_read.hxx"
#include "imports.hxx"
#include "../../../approx_objective/Approx_Objective.hxx"

//Compute dx and dy of the central sdp as the standard sdp_solver does in predictor phase. 
//Correspond to - H^-1_xx Del_p L_mu in Eq(13).
//Return: void.
//Update dx, dy. 
void internal_predictor_direction(
		const Block_Info &block_info, const SDP &sdp, const Dynamical_Solver &solver,
		const Block_Diagonal_Matrix &schur_complement_cholesky,
		const Block_Matrix &schur_off_diagonal,
		const Block_Diagonal_Matrix &X_cholesky, const El::BigFloat beta,
		const El::BigFloat &mu, const Block_Vector &primal_residue_p,
		const El::DistMatrix<El::BigFloat> &Q, Block_Vector &grad_x, Block_Vector &grad_y, Block_Vector &dx, Block_Vector &dy, Block_Diagonal_Matrix &R);

void internal_corrector_direction(
		const Block_Info &block_info, const SDP &sdp, const Dynamical_Solver &solver,
		const Block_Diagonal_Matrix &schur_complement_cholesky,
		const Block_Matrix &schur_off_diagonal,
		const Block_Diagonal_Matrix &X_cholesky, const El::BigFloat beta,
		const El::BigFloat &mu, const Block_Vector &primal_residue_p,
		const El::DistMatrix<El::BigFloat> &Q, Block_Vector &grad_x, Block_Vector &grad_y, Block_Vector &dx, Block_Vector &dy, Block_Diagonal_Matrix &dX, Block_Diagonal_Matrix &dY, Block_Diagonal_Matrix &R);

void internal_predictor_direction_dxdydXdY(
	const Block_Info &block_info, const SDP &sdp, const Dynamical_Solver &solver,
	const Block_Diagonal_Matrix &schur_complement_cholesky,
	const Block_Matrix &schur_off_diagonal,
	const Block_Diagonal_Matrix &X_cholesky, const El::BigFloat beta,
	const El::BigFloat &mu, const Block_Vector &primal_residue_p,
	const El::DistMatrix<El::BigFloat> &Q, Block_Vector &grad_x, Block_Vector &grad_y, Block_Vector &dx, Block_Vector &dy, Block_Diagonal_Matrix &dX, Block_Diagonal_Matrix &dY, Block_Diagonal_Matrix &R);


//Compute the linear difference of the lagrangians of two sdps 
//The same as the calculation done by Approx_Objective
El::BigFloat compute_delta_lag(const SDP &d_sdp, const Dynamical_Solver &solver);

El::BigFloat compute_lag(const El::BigFloat mu, const Block_Diagonal_Matrix &X_cholesky, const Dynamical_Solver &solver);

//Given delta_p(sdp) , compute the (delta_x, delta_y) = H_xx^-1 H_xp delta_p
//as shown in Eq(15). 
//Return: void. 
//Update: hess_xp = H_xp = (RHS(p1)/alpha, RHS(p2)/alpha, ... ), stored to compute the second term on the LHS of Eq(13) 
//        delta_x_y = - H^-1_xx H_xp = - H^-1_xx hess_xp 
void mixed_hess(
		const Block_Info &block_info, const SDP &d_sdp,
		const Block_Vector &x, const Block_Vector &y,
		const Block_Diagonal_Matrix &schur_complement_cholesky,
		const Block_Matrix &schur_off_diagonal,
		const El::DistMatrix<El::BigFloat> &Q,
		const El::BigFloat &alpha,
		std::vector<std::pair<Block_Vector, Block_Vector>> &hess_xp,
		std::vector<std::pair<Block_Vector, Block_Vector>> &delta_x_y);

//Given objectives on a grid in the external parameter (p) space,
//where ePlus correspond to (+e_i), eMinus corresponds to (-e_i) and eSum corresponds to (e_i+e_j) directions respectively, 
//Compute H_pp and Del_p(L_mu) to solve Eq(13).
//Return: void.
//Update: hess = H_pp ,
//        grad = Del_p(L_mu).
void external_grad_hessian(const El::Matrix<El::BigFloat> &ePlus, 
		const El::Matrix<El::BigFloat> &eMinus, 
		const El::Matrix<El::BigFloat> &eSum,
		const El::BigFloat &alpha,
		El::Matrix<El::BigFloat> &grad,
		El::Matrix<El::BigFloat> &hess);

//void external_grad(const El::Matrix<El::BigFloat> &ePlus,
//	const El::Matrix<El::BigFloat> &eMinus,
//	const El::BigFloat &alpha,
//	El::Matrix<El::BigFloat> &grad);

//This function is the same as in the usual SDP_Solver, 
//modified to take the argument 'solver' of the right type. 
//Currently not used in the dynamical_step 
void compute_search_direction(
		const Block_Info &block_info, const SDP &sdp, const Dynamical_Solver &solver,
		const Block_Diagonal_Matrix &schur_complement_cholesky,
		const Block_Matrix &schur_off_diagonal,
		const Block_Diagonal_Matrix &X_cholesky, const El::BigFloat beta,
		const El::BigFloat &mu, const Block_Vector &primal_residue_p,
		const bool &is_corrector_phase, const El::DistMatrix<El::BigFloat> &Q,
		Block_Vector &dx, Block_Diagonal_Matrix &dX, Block_Vector &dy,
		Block_Diagonal_Matrix &dY);

void compute_find_zeros(
		const El::BigFloat &duality_gap, const El::BigFloat &primal_objective,
		bool &find_zeros);


void find_zero_step (const El::BigFloat &thresh, const int &max_it, const El::BigFloat &step_size_max,
		El::Matrix<El::BigFloat> &hess, const El::Matrix<El::BigFloat> &grad, bool &find_zeros, 
		El::Matrix<El::BigFloat> &external_step, const El::BigFloat &quadratic_const);





void BFGS_update_hessian(const int n_parameters,
		const El::Matrix<El::BigFloat> &grad_p_diff,
		const El::Matrix<El::BigFloat> &last_it_step,
		El::Matrix<El::BigFloat> &hess_bfgs
		);


void read_sdp_grid(
	const Dynamical_Solver_Parameters &dynamical_parameters,
	const Block_Info &block_info,
	const SDP &sdp, const El::Grid &grid,
	Timers &timers,

	Block_Diagonal_Matrix & schur_complement_cholesky,
	Block_Matrix & schur_off_diagonal,

	El::DistMatrix<El::BigFloat> & Q,

	Block_Vector & x,
	Block_Vector & y,

	const Block_Diagonal_Matrix &X_cholesky,

	int n_external_parameters,

	El::Matrix<El::BigFloat> & eplus,
	El::Matrix<El::BigFloat> & eminus,
	El::Matrix<El::BigFloat> & esum,

	El::Matrix<El::BigFloat> & Lpu,

	std::vector<std::pair<Block_Vector, Block_Vector>> & H_xp,
	std::vector<std::pair<Block_Vector, Block_Vector>> & Delta_xy)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	El::Zero(eplus);
	El::Zero(eminus);
	El::Zero(esum);
	if (dynamical_parameters.new_sdp_path.extension() == ".nsv")
	{
		for (auto &filename : read_file_list(dynamical_parameters.new_sdp_path))
		{
			//Assume that the filename takes the form "plus_i","minus_i" and "sum_i_j", f
			//standing for the change in positive e_i, negative e_i and (e_i + e_j) directions respectively 
			std::string file_name = filename.stem().string();
			std::vector<std::string> directions;
			boost::algorithm::split(directions, file_name, boost::is_any_of("_"));
			SDP new_sdp(filename, block_info, grid), d_sdp(new_sdp);
			Axpy(El::BigFloat(-1), sdp, d_sdp);

			//std::cout << "rank=" << El::mpi::Rank() << " pid=" << getpid() << " before approx_obj \n" << std::flush;

			Approx_Objective approx_obj(block_info, sdp, d_sdp, x, y, X_cholesky,
				schur_complement_cholesky,
				schur_off_diagonal, Q);

			//std::cout << "rank=" << El::mpi::Rank() << " after approx_obj \n" << std::flush;

			if (directions[0] == "plus")
			{
				mixed_hess(block_info, d_sdp, x, y, schur_complement_cholesky, schur_off_diagonal, Q, dynamical_parameters.alpha, H_xp, Delta_xy);
				eplus(std::stoi(directions[1])) = approx_obj.d_objective;//+ approx_obj.dd_objective; 
																		 //compute_delta_lag(d_sdp, *this);

				Lpu(std::stoi(directions[1])) = approx_obj.Lag_pu / dynamical_parameters.alpha;
			}
			else if (directions[0] == "minus")
			{
				eminus(std::stoi(directions[1])) = approx_obj.d_objective;//+ approx_obj.dd_objective; 
																		  //compute_delta_lag(d_sdp,*this); 
			}
			else if (directions[0] == "sum")
			{
				El::BigFloat tempt = approx_obj.d_objective;//+ approx_obj.dd_objective;
				esum(std::stoi(directions[1]), std::stoi(directions[2])) = tempt;
				esum(std::stoi(directions[2]), std::stoi(directions[1])) = tempt;
				//compute_delta_lag(d_sdp,*this); 
			}
		}
	}
	else
	{
		throw std::invalid_argument("A list of perturbed sdp files are required");
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	if (El::mpi::Rank() == 0)
		std::cout << "read_sdp_grid : " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]\n" << std::flush;

	return;
}

void print_matrix(const El::Matrix<El::BigFloat> & matrix);
void print_vector(const El::Matrix<El::BigFloat> & vec);

void compute_ellipse_boundary(const El::Matrix<El::BigFloat> &H, 
	const El::Matrix<El::BigFloat> &g, 
	El::BigFloat &f0,
	const El::Matrix<El::BigFloat> &search_direction,
	El::Matrix<El::BigFloat> & result_plus,
	El::Matrix<El::BigFloat> & result_minus,
	El::BigFloat & lambda)
{
	El::Matrix<El::BigFloat> invH_e = search_direction;
	El::LinearSolve(H, invH_e);
	El::BigFloat e_invH_e = El::Dot(invH_e, search_direction);

	El::Matrix<El::BigFloat> invH_g = g;
	El::LinearSolve(H, invH_g);
	El::BigFloat g_invH_g = El::Dot(invH_g, g);

	lambda = (g_invH_g - 2 * f0) / (e_invH_e);
	El::BigFloat lambda_sqrt;
	if (lambda < 0)
		lambda_sqrt = 0;
	else lambda_sqrt = El::Sqrt(lambda);

	result_plus = invH_e;
	result_plus *= lambda_sqrt;
	result_plus -= invH_g;

	result_minus = invH_e;
	result_minus *= -lambda_sqrt;
	result_minus -= invH_g;


	if (El::mpi::Rank() == 0) std::cout << std::setprecision(3);

	return;
}

bool positivitize_matrix(El::Matrix<El::BigFloat> & matrix)
{
	int dim = matrix.Width();
	bool flippedQ = false;

	//Flip the sign of Hessian if determinant is negative 
	typedef El::Base<El::BigFloat> Real;
	El::Matrix<Real> w(dim, 1);
	El::Zero(w);
	El::Matrix<El::BigFloat> Q(matrix);
	El::Matrix<El::BigFloat> tempt(matrix);
	El::Matrix<El::BigFloat> diag(matrix);
	El::Zero(diag);
	El::HermitianEig(El::LOWER, matrix, w, Q);//,ctrl);
	for (int i = 0; i < dim; i++) {
		if (w(i) < 0) flippedQ = true;
		diag(i, i) = El::Abs(w(i));
	}
	El::Gemm(El::NORMAL, El::TRANSPOSE, El::BigFloat(1), diag, Q, tempt);
	El::Gemm(El::NORMAL, El::NORMAL, El::BigFloat(1), Q, tempt, matrix);

	return flippedQ;
}


void BFGS_update_hessian(const int n_parameters,
	const El::Matrix<El::BigFloat> &grad_p_diff,
	const El::Matrix<El::BigFloat> &last_it_step,
	El::Matrix<El::BigFloat> &hess_bfgs
);


El::BigFloat LA_vector_matrix_vector(const El::Matrix<El::BigFloat> &vec1,
	const El::Matrix<El::BigFloat> &mat,
	const El::Matrix<El::BigFloat> &vec2
	)
{
	El::Matrix<El::BigFloat> mat_vec2(vec2);
	El::Gemv(El::NORMAL, El::BigFloat(1), mat, vec2, El::BigFloat(0), mat_vec2);
	return El::Dot(vec1, mat_vec2);
}

void LA_matrix_vector(const El::Matrix<El::BigFloat> &mat,
	const El::Matrix<El::BigFloat> &vec,
	El::Matrix<El::BigFloat> &result)
{
	result = vec;
	El::Gemv(El::NORMAL, El::BigFloat(1), mat, vec, El::BigFloat(0), result);
}

void LA_x1_vec1_plus_x2_vec2(const El::BigFloat & x1, const El::Matrix<El::BigFloat> &vec1,
	const El::BigFloat & x2, const El::Matrix<El::BigFloat> &vec2,
	El::Matrix<El::BigFloat> &result)
{
	El::Matrix<El::BigFloat> x2_vec2 = vec2;
	x2_vec2 *= x2;

	result = vec1;
	result *= x1;
	result += x2_vec2;
}

// return whether the update is exact
bool BFGS_partial_update_hessian(const El::BigFloat & reduction_factor,
	const El::Matrix<El::BigFloat> &y,
	const El::Matrix<El::BigFloat> &s,
	El::Matrix<El::BigFloat> &B)
{
	El::BigFloat sy = El::Dot(s, y);

	if (sy >= 0)
	{
		BFGS_update_hessian(B.Width(), y, s, B);
		return true;
	}

	El::BigFloat b=LA_vector_matrix_vector(s,B,s);

	El::Matrix<El::BigFloat> y2;
	LA_matrix_vector(B,s,y2);

	El::BigFloat y2_coeff = reduction_factor - sy / b;

	El::Matrix<El::BigFloat> y_shifted;
	LA_x1_vec1_plus_x2_vec2(El::BigFloat(1),y, y2_coeff,y2, y_shifted);

	BFGS_update_hessian(B.Width(), y_shifted, s, B);
	return false;
}


bool BFGS_update_hessian(
	const El::Matrix<El::BigFloat> &grad_p_diff,
	const El::Matrix<El::BigFloat> &last_it_step,
	El::Matrix<El::BigFloat> &hess_bfgs,
	bool update_only_when_positive)
{
	El::Matrix<El::BigFloat> hess_bfgs_save(hess_bfgs);

	BFGS_update_hessian(hess_bfgs.Width(), grad_p_diff, last_it_step, hess_bfgs);

	bool flippedQ = positivitize_matrix(hess_bfgs);

	if (update_only_when_positive && flippedQ)
		hess_bfgs = hess_bfgs_save;

	return flippedQ;
}


void external_grad_hessian(const El::Matrix<El::BigFloat> &ePlus,
	const El::Matrix<El::BigFloat> &eMinus,
	const El::Matrix<El::BigFloat> &eSum,
	const El::BigFloat &alpha,
	El::Matrix<El::BigFloat> &grad,
	El::Matrix<El::BigFloat> &hess);


void external_grad(const El::Matrix<El::BigFloat> &ePlus,
	const El::Matrix<El::BigFloat> &eMinus,
	const El::BigFloat &alpha,
	El::Matrix<El::BigFloat> &grad)
{
	grad = ePlus;
	grad *= El::BigFloat(1) / alpha;
}


extern bool compute_ext_step_only_once;
extern bool recompute_ext_during_re_aiming;

extern int max_climbing;

extern bool update_hess_only_positive;

extern bool use_Lpu_mu_correction;

extern double step_min_threshold;
extern double step_max_threshold;

extern double navigator_shift;
extern double dgap_near_optimal_threshold;
extern double beta_scan_begin;
extern double beta_scan_end;
extern double beta_scan_step;
extern bool use_gradp_for_BFGS_update;
extern double rescale_initial_hess;
extern double BFGS_partial_update_reduction;
extern bool compare_BFGS_gradient_at_same_mu;
extern double beta_for_mu_logdetX;


void compute_grad_p_grad_mixed_Hpp_Hmixed(const Dynamical_Solver_Parameters &dynamical_parameters,
	const El::Matrix<El::BigFloat> & eplus, const El::Matrix<El::BigFloat> & eminus, const El::Matrix<El::BigFloat> & esum,
	const std::vector<std::pair<Block_Vector, Block_Vector>> & H_xp, const std::vector<std::pair<Block_Vector, Block_Vector>> & Delta_xy,
	const Block_Vector & internal_dx, const Block_Vector & internal_dy,
	El::Matrix<El::BigFloat> & grad_p, El::Matrix<El::BigFloat> & grad_mixed, El::Matrix<El::BigFloat> & grad_corrected,
	El::Matrix<El::BigFloat> & Lpu, El::BigFloat & mu,
	El::Matrix<El::BigFloat> & hess_pp, El::Matrix<El::BigFloat> & hess_mixed, El::Matrix<El::BigFloat> & hess_Exact
	)
{
	int dim_ext_p = dynamical_parameters.n_external_parameters;

	if (dynamical_parameters.use_exact_hessian)
	{
		external_grad_hessian(eplus, eminus, esum, dynamical_parameters.alpha, grad_p, hess_pp);
	}
	else
	{
		//std::cout << "compute grad: " << '\n' << std::flush;
		external_grad(eplus, eminus, dynamical_parameters.alpha, grad_p);
	}

	for (int i = 0; i < dim_ext_p; i++)
	{
		for (int j = 0; j < dim_ext_p; j++)
		{
			// The minus sign compensate the minus sign when calculating Delta_xy in Eq(15)
			hess_mixed(i, j) = -(dot(H_xp.at(i).first, Delta_xy.at(j).first) + dot(H_xp.at(i).second, Delta_xy.at(j).second));
		}
	}

	for (int i = 0; i < dim_ext_p; i++)
	{
		// The minus sign compensates the minus sign when calculating the internal step 
		grad_mixed(i) = (dot(H_xp.at(i).first, internal_dx) + dot(H_xp.at(i).second, internal_dy));
	}

	if (dynamical_parameters.use_exact_hessian)
	{
		hess_Exact = hess_pp;
		hess_Exact -= hess_mixed;
	}

	if (use_Lpu_mu_correction)
	{
		grad_corrected = Lpu;
		grad_corrected *= -mu;
		grad_corrected += grad_p;
	}
	else
	{
		grad_corrected = grad_mixed;
		grad_corrected += grad_p;
	}
}

void read_prev_grad_step_hess(const Dynamical_Solver_Parameters &dynamical_parameters,
	El::Matrix<El::BigFloat> & prev_grad, El::Matrix<El::BigFloat> & prev_step, El::Matrix<El::BigFloat> & prev_BFGS)
{
	int dim_ext_p = dynamical_parameters.n_external_parameters;

	for (int i = 0; i < dim_ext_p; i++)
	{
		prev_grad(i, 0) = dynamical_parameters.prev_grad[i];
		prev_step(i, 0) = dynamical_parameters.prev_step[i];
	}

	for (int i = 0; i < dim_ext_p; i++)
	{
		for (int j = 0; j < dim_ext_p; j++)
		{
			prev_BFGS(i, j) = dynamical_parameters.hess_BFGS[i * dim_ext_p + j];
		}
	}
}


void save_load_beta_scan_best_state(Block_Vector & dx, Block_Vector & dy,
	Block_Diagonal_Matrix & dX, Block_Diagonal_Matrix & dY,
	El::BigFloat & primal_step_length, El::BigFloat & dual_step_length,
	El::BigFloat & beta,
	El::Matrix<El::BigFloat> & hess_BFGS, El::Matrix<El::BigFloat> & grad_mixed,

	Block_Vector & dx_best, Block_Vector & dy_best,
	Block_Diagonal_Matrix & dX_best, Block_Diagonal_Matrix & dY_best,
	El::BigFloat & primal_step_length_best, El::BigFloat & dual_step_length_best,
	El::BigFloat & beta_best,
	El::Matrix<El::BigFloat> & hess_BFGS_best, El::Matrix<El::BigFloat> & grad_mixed_best
)
{
	dX = dX_best;
	dY = dY_best;
	dx = dx_best;
	dy = dy_best;
	primal_step_length = primal_step_length_best;
	dual_step_length = dual_step_length_best;
	beta = beta_best;
	hess_BFGS = hess_BFGS_best;
	grad_mixed = grad_mixed_best;
}


void extrapolate_gradient_as_target_mu(
	const El::BigFloat & primal_step_length, const El::BigFloat & dual_step_length,
	const El::Matrix<El::BigFloat> & grad_p, const El::Matrix<El::BigFloat> & grad_mixed_best, El::Matrix<El::BigFloat> & grad_BFGS)
{
	El::BigFloat step_length_avg = (primal_step_length + dual_step_length) / 2;
	grad_BFGS = grad_mixed_best;
	grad_BFGS *= step_length_avg;
	grad_BFGS += grad_p;
}



///////////////// member function of Dynamical_Solver //////////////////////////

// compute dX,dY based on dx,dy
void Dynamical_Solver::compute_dXdY(
	const bool &is_primal_and_dual_feasible,
	const Block_Info &block_info,
	const SDP &sdp, const El::Grid &grid,
	const Block_Diagonal_Matrix &X_cholesky,
	const Block_Diagonal_Matrix &Y_cholesky,
	Timers &timers,

	Block_Vector & dx, Block_Vector & dy,
	Block_Diagonal_Matrix & dX, Block_Diagonal_Matrix & dY,
	Block_Diagonal_Matrix & R,
	El::BigFloat &primal_step_length, El::BigFloat &dual_step_length,
	El::BigFloat &step_length_reduction
)
{
	//Block_Diagonal_Matrix dX(X), dY(Y);
	// dX = PrimalResidues + \sum_p A_p dx[p]
	constraint_matrix_weighted_sum(block_info, sdp, dx, dX);
	dX += primal_residues;

	// dY = Symmetrize(X^{-1} (R - dX Y))
	multiply(dX, Y, dY);
	dY -= R;
	cholesky_solve(X_cholesky, dY);
	dY.symmetrize();
	dY *= El::BigFloat(-1);

	// Compute step-lengths that preserve positive definiteness of X, Y
	primal_step_length
		= step_length(X_cholesky, dX, step_length_reduction,
			"run.step.stepLength(XCholesky)", timers);

	dual_step_length
		= step_length(Y_cholesky, dY, step_length_reduction,
			"run.step.stepLength(YCholesky)", timers);

	if (is_primal_and_dual_feasible)
	{
		primal_step_length = El::Min(primal_step_length, dual_step_length);
		dual_step_length = primal_step_length;
	}
}

// compute external dx,dy based on external_step
void Dynamical_Solver::compute_external_dxdy(
	const Dynamical_Solver_Parameters &dynamical_parameters,

	Block_Vector & internal_dx, Block_Vector & internal_dy,
	Block_Vector & dx, Block_Vector & dy,
	Block_Diagonal_Matrix & R,

	El::BigFloat &delta_lambda,
	El::Matrix<El::BigFloat> & external_step,
	std::vector<std::pair<Block_Vector, Block_Vector>> & Delta_xy
)
{
	dx = internal_dx;
	dy = internal_dy;
	// RHS of Eq(12)
	//    - H^-1_xx Del_x L_mu - H^-1_xx H_xp dp 
	// =  internal_dx, internal_dy + Delta_xy . external_step
	for (int i = 0; i<dynamical_parameters.n_external_parameters; i++)
	{
		for (size_t block = 0; block < x.blocks.size(); ++block)
		{
			if (dynamical_parameters.find_boundary)
			{
				dx.blocks[block] *= (1.0 + delta_lambda / lag_multiplier_lambda);
			}
			El::Axpy(external_step(i), Delta_xy.at(i).first.blocks[block], dx.blocks[block]);
		}
		for (size_t block = 0; block < dy.blocks.size(); ++block)
		{
			if (dynamical_parameters.find_boundary)
			{
				dy.blocks[block] *= (1.0 + delta_lambda / lag_multiplier_lambda);
			}
			El::Axpy(external_step(i), Delta_xy.at(i).second.blocks[block], dy.blocks[block]);
		}
	}

	if (dynamical_parameters.find_boundary)
	{
		primal_residues *= (1.0 + delta_lambda / lag_multiplier_lambda);
		R *= (1.0 + delta_lambda / lag_multiplier_lambda);
	}
}



// This function will update dx,dy,dX,dY and step_length_primal, step_length_dual
// If external_step_Q=true, dx,dy,dX,dY hopping step computed for new sdp.
// Otherwise this function set dx,dy to be internal_dx, internal_dy and update dX,dY
void Dynamical_Solver::compute_external_dxdydXdY(
	const bool &is_primal_and_dual_feasible,
	const Dynamical_Solver_Parameters &dynamical_parameters,
	const Block_Info &block_info,
	const SDP &sdp, const El::Grid &grid,
	const Block_Diagonal_Matrix &X_cholesky,
	const Block_Diagonal_Matrix &Y_cholesky,
	Timers &timers,

	Block_Vector & internal_dx, Block_Vector & internal_dy,
	Block_Vector & dx, Block_Vector & dy,
	Block_Diagonal_Matrix & dX, Block_Diagonal_Matrix & dY,
	Block_Diagonal_Matrix & R,

	El::BigFloat &delta_lambda,
	El::Matrix<El::BigFloat> & external_step,
	std::vector<std::pair<Block_Vector, Block_Vector>> & Delta_xy,
	El::BigFloat &primal_step_length, El::BigFloat &dual_step_length,
	El::BigFloat &step_length_reduction
)
{
	compute_external_dxdy(dynamical_parameters, internal_dx, internal_dy, dx, dy, R, delta_lambda, external_step, Delta_xy);

	compute_dXdY(is_primal_and_dual_feasible, block_info, sdp, grid,
		X_cholesky, Y_cholesky, timers,
		dx, dy, dX, dY, R, primal_step_length, dual_step_length, step_length_reduction);
}


void Dynamical_Solver::internal_step(
	const Dynamical_Solver_Parameters &dynamical_parameters,
	const Block_Info &block_info,
	const SDP &sdp, const El::Grid &grid,
	const Block_Diagonal_Matrix &X_cholesky,
	const Block_Diagonal_Matrix &Y_cholesky,
	Timers &timers,

	const Block_Diagonal_Matrix &schur_complement_cholesky,
	const Block_Matrix &schur_off_diagonal,
	const El::DistMatrix<El::BigFloat> &Q,

	Block_Vector & dx, Block_Vector & dy,
	Block_Diagonal_Matrix & dX, Block_Diagonal_Matrix & dY,
	Block_Diagonal_Matrix & R,
	Block_Vector &grad_x, Block_Vector &grad_y,

	const Block_Vector &primal_residue_p, El::BigFloat &mu,

	const bool &is_primal_and_dual_feasible,
	El::BigFloat &beta,
	El::BigFloat &primal_step_length, El::BigFloat &dual_step_length,
	El::BigFloat &step_length_reduction
	)
{
	internal_predictor_direction_dxdydXdY(block_info, sdp, *this, schur_complement_cholesky,
		schur_off_diagonal, X_cholesky, beta,
		mu, primal_residue_p, Q, grad_x, grad_y, dx, dy, dX, dY, R);

	internal_corrector_direction(block_info, sdp, *this, schur_complement_cholesky,
		schur_off_diagonal, X_cholesky, beta,
		mu, primal_residue_p, Q, grad_x, grad_y, dx, dy, dX, dY, R);

	compute_dXdY(is_primal_and_dual_feasible, block_info, sdp, grid,
		X_cholesky, Y_cholesky, timers,
		dx, dy, dX, dY, R, primal_step_length, dual_step_length, step_length_reduction);

	execute_step(dx, dy, dX, dY, primal_step_length, dual_step_length);
}


void Dynamical_Solver::execute_step(
	Block_Vector & dx, Block_Vector & dy,
	Block_Diagonal_Matrix & dX, Block_Diagonal_Matrix & dY,
	El::BigFloat &primal_step_length,
	El::BigFloat &dual_step_length
)
{
	for (size_t block = 0; block < x.blocks.size(); ++block)
	{
		El::Axpy(primal_step_length, dx.blocks[block], x.blocks[block]);
	}
	for (size_t block = 0; block < dy.blocks.size(); ++block)
	{
		El::Axpy(dual_step_length, dy.blocks[block], y.blocks[block]);
	}

	dX *= primal_step_length;
	dY *= dual_step_length;
	X += dX;
	Y += dY;
}


void Dynamical_Solver::strategy_hess_BFGS(const Dynamical_Solver_Parameters &dynamical_parameters, int n_external_parameters,
	bool lowest_mu_Q,
	El::Matrix<El::BigFloat> & grad_p, El::Matrix<El::BigFloat> & grad_mixed, El::Matrix<El::BigFloat> & grad_corrected,
	El::Matrix<El::BigFloat> & Lpu, El::BigFloat & mu,
	El::Matrix<El::BigFloat> & hess_pp, El::Matrix<El::BigFloat> & hess_mixed, El::Matrix<El::BigFloat> & hess_Exact,

	El::Matrix<El::BigFloat> & prev_BFGS, El::Matrix<El::BigFloat> & prev_step, El::Matrix<El::BigFloat> & prev_grad,
	El::Matrix<El::BigFloat> & hess_BFGS_lowest_mu
)
{
	if (dynamical_parameters.use_exact_hessian)
	{
		if (positivitize_matrix(hess_Exact) == false)
			if (El::mpi::Rank() == 0) std::cout << "flip the sign of hessian\n" << std::flush;

		hess_BFGS = hess_Exact;
	}
	else
	{
		if (El::mpi::Rank() == 0)std::cout << "using BFGS: " << '\n' << std::flush;

		if (dynamical_parameters.use_Hmixed_for_BFGS) // I will let simpleboot control whether hess_mixed will be used for hess_BFGS
		{
			El::Zeros(hess_BFGS, n_external_parameters, n_external_parameters);
			hess_BFGS -= hess_mixed;
			positivitize_matrix(hess_BFGS);
			hess_BFGS *= El::BigFloat(rescale_initial_hess);
		}
		else
		{
			hess_BFGS = prev_BFGS;
		}

		if (lowest_mu_Q == false) hess_BFGS = hess_BFGS_lowest_mu;

		// update hess_BFGS
		if (dynamical_parameters.total_iterations > 0 && lowest_mu_Q)
		{
			El::Matrix<El::BigFloat> grad_diff;
			if (use_gradp_for_BFGS_update)
				grad_diff = grad_p;
			else
				grad_diff = grad_corrected;

			grad_diff -= prev_grad;

			if (BFGS_partial_update_reduction > 0)
			{
				bool exact_update_Q = BFGS_partial_update_hessian(El::BigFloat(BFGS_partial_update_reduction),
					grad_diff, prev_step, hess_BFGS);
				if (!exact_update_Q)
					if (El::mpi::Rank() == 0)
						std::cout << "New hessian is non-positive. partial update. \n" << std::flush;
			}
			else
			{
				bool flippedQ = BFGS_update_hessian(grad_diff, prev_step, hess_BFGS, update_hess_only_positive);
				if (flippedQ)
					if (update_hess_only_positive)
					{
						if (El::mpi::Rank() == 0) std::cout << "New hessian is non-positive. Rollback to prev_hess. \n" << std::flush;
					}
					else
					{
						if (El::mpi::Rank() == 0) std::cout << "New hessian is non-positive (still updated). \n" << std::flush;
					}

				if (flippedQ == false || update_hess_only_positive == false)
					hess_BFGS_updateQ = true;
			}

			hess_BFGS_lowest_mu = hess_BFGS;

			//if (El::mpi::Rank() == 0) std::cout << "hess_BFGS_lowest_mu saved : " << std::flush;
			//print_matrix(hess_BFGS_lowest_mu);
		}
	}


	//if (El::mpi::Rank() == 0) std::cout << "hess_BFGS_lowest_mu : " << std::flush;
	//print_matrix(hess_BFGS_lowest_mu);

	//if (El::mpi::Rank() == 0) std::cout << "hess_BFGS : " << std::flush;
	//print_matrix(hess_BFGS);
}


El::BigFloat compute_lag(const El::BigFloat mu, const Block_Diagonal_Matrix &X_cholesky, const Dynamical_Solver &solver);

void Dynamical_Solver::strategy_findboundary_extstep(
	const Block_Diagonal_Matrix &X_cholesky, 
	const std::size_t &total_psd_rows,
	const El::BigFloat & mu,
	const El::BigFloat & beta,

	int n_external_parameters,
	const Dynamical_Solver_Parameters &dynamical_parameters,
	bool lowest_mu_Q,

	El::Matrix<El::BigFloat> & grad_corrected,
	El::Matrix<El::BigFloat> & grad_p,
	El::Matrix<El::BigFloat> & grad_mixed,

	El::Matrix<El::BigFloat> & external_step,
	El::Matrix<El::BigFloat> & external_step_save,
	bool & external_step_specified_Q
)
{
	El::BigFloat lag0 = compute_lag(0, X_cholesky, *this);

	El::BigFloat beta_lag;
	if (beta_for_mu_logdetX < 0)
	{
		beta_lag = beta;
	}
	else
	{
		beta_lag = El::BigFloat(beta_for_mu_logdetX);
	}

	El::BigFloat lag = compute_lag(beta_lag*mu, X_cholesky, *this);

	El::BigFloat lag_shifted = lag;
	lag_shifted -= total_psd_rows * mu * El::BigFloat(navigator_shift);

	if (El::mpi::Rank() == 0)
	{
		std::cout << "lag_shifted  = " << lag_shifted << '\n' << std::flush;
		std::cout << "logdetX  = " << lag - lag0 << '\n' << std::flush;
	}

	//find_zero_step(10^(-10), 100, dynamical_parameters.update_sdp_threshold_max,
	//		Hpp_minus_Hmixed, grad_mixed, find_zeros, external_step, lag);

	El::Matrix<El::BigFloat> search_direction(n_external_parameters, 1);
	for (int i = 0; i < n_external_parameters; i++)
	{
		search_direction(i, 0) = dynamical_parameters.search_direction[i];
	}

	if (compute_ext_step_only_once == true && external_step_specified_Q == true &&
		(recompute_ext_during_re_aiming == false || lowest_mu_Q == false)
		)
	{
		// hess_BFGS will not be updated, but grad_BFGS will be updated
		external_step = external_step_save;
	}
	else
	{
		El::BigFloat lambda;
		El::Matrix<El::BigFloat> external_step_plus, external_step_minus;

		compute_ellipse_boundary(hess_BFGS, grad_corrected, lag_shifted, search_direction,
			external_step_plus, external_step_minus, lambda);
		external_step = external_step_plus;

		if (El::mpi::Rank() == 0)
		{
			if (use_Lpu_mu_correction)
				std::cout << "\nLpu corrected : \n";

			std::cout << "BFGS hess =\n";
			print_matrix(hess_BFGS);

			std::cout << "grad_corrected = ";
			print_vector(grad_corrected);
			std::cout << "\n";

			std::cout << "grad_p = ";
			print_vector(grad_p);
			std::cout << "\n";

			std::cout << "grad_mixed = ";
			print_vector(grad_mixed);
			std::cout << "\n";

			std::cout << " lambda=" << lambda << "\n";
			if (lambda < 0)
				std::cout << "ext-step_zero=";
			else
				std::cout << "ext-step_plus=";

			print_vector(external_step_plus);
			std::cout << "\n" << std::flush;
		}

		external_step_save = external_step;
		external_step_specified_Q = true;
	}

	external_step_size = El::Nrm2(external_step);
}



void Dynamical_Solver::strategy_update_grad_BFGS(
	El::BigFloat &primal_step_length, El::BigFloat &dual_step_length,
	El::Matrix<El::BigFloat> & grad_p, El::Matrix<El::BigFloat> & grad_mixed, El::Matrix<El::BigFloat> & grad_BFGS
)
{
	if (compare_BFGS_gradient_at_same_mu)
	{
		extrapolate_gradient_as_target_mu(primal_step_length, primal_step_length, grad_p, grad_mixed, grad_BFGS);
	}
	else if (use_gradp_for_BFGS_update)
	{
		grad_BFGS = grad_p;
	}
	else
	{
		grad_BFGS = grad_mixed;
		grad_BFGS += grad_p;
	}
}