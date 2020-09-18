/*
 * QueryTest.cpp
 *
 *  Created on: Jan 27, 2016
 *      Author: Michael Wegner
 *
 * Copyright (c) 2016 Michael Wegner and Matthias Wolf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../algorithm/CRPQuery.h"
#include "../algorithm/CRPQueryUni.h"
#include "../algorithm/ParallelCRPQuery.h"

#include "../datastructures/Graph.h"
#include "../datastructures/OverlayGraph.h"
#include "../datastructures/OverlayWeights.h"
#include "../datastructures/QueryResult.h"
#include "../io/GraphIO.h"
#include "../metrics/Metric.h"
#include "../metrics/DistanceFunction.h"
#include "../metrics/HopFunction.h"
#include "../metrics/TimeFunction.h"
#include "../metrics/CostFunction.h"

#include "../timer.h"
#include "../constants.h"

#include <iostream>
#include <memory>
#include <iomanip>
#include <fstream>
#include <random>
#include <functional>

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 6) {
		std::cout << argv[0] << " numQueries pathToGraph pathToOverlayGraph pathToMetric metricType" << std::endl;
		return 1;
	}

	std::string numQueryString = argv[1];
	CRP::count numQueries = std::stoi(numQueryString);
	std::string graphFile = argv[2];
	std::string overlayGraphFile = argv[3];
	std::string metricPath = argv[4];
	std::string metricType = argv[5];

	cout << "Reading graph" << endl;
	CRP::Graph graph;
	CRP::GraphIO::readGraph(graph, graphFile);

	cout << "Reading overlay graph" << endl;
	CRP::OverlayGraph overlayGraph;
	CRP::GraphIO::readOverlayGraph(overlayGraph, overlayGraphFile);

	cout << "Reading metric" << endl;
	vector<CRP::Metric> metrics(1);
	if (metricType == "dist") {
		std::ifstream stream(metricPath);
		CRP::Metric::read(stream, metrics[0], std::unique_ptr<CRP::CostFunction>(new CRP::DistanceFunction()));
		stream.close();
	} else if (metricType == "hop") {
		std::ifstream stream(metricPath);
		CRP::Metric::read(stream, metrics[0], std::unique_ptr<CRP::CostFunction>(new CRP::HopFunction()));
		stream.close();
	} else if (metricType == "time") {
		std::ifstream stream(metricPath);
		CRP::Metric::read(stream, metrics[0], std::unique_ptr<CRP::CostFunction>(new CRP::TimeFunction()));
		stream.close();
	} else {
		std::cout << "ERROR: Unknown metic type " << metricType << std::endl;
		return 1;
	}

	std::cout << "Done" << std::endl;

	std::mt19937 rand;
	auto vertex_rand = std::bind(std::uniform_int_distribution<CRP::index>(0, graph.numberOfVertices()-1),
	                           mt19937(get_micro_time()));

	CRP::PathUnpacker pathUnpacker(graph, overlayGraph, metrics);
	CRP::CRPQueryUni query(graph, overlayGraph, metrics, pathUnpacker);
		
	std::vector<std::pair<CRP::index, CRP::index>> queries(numQueries);
	for (CRP::index i = 0; i < numQueries; ++i) {
		queries[i] = std::make_pair(vertex_rand(), vertex_rand());
	}

	std::vector<CRP::weight> pathTime(numQueries);

	CRP::QueryResult result;

	std::cout << "Running uni queries" << std::endl;
	CRP::index i = 0;
	double avgPathTime = 0;
	for (std::pair<CRP::index, CRP::index> &q : queries) {
		CRP::index source = q.first;
		CRP::index target = q.second;

	        result = query.vertexQuery(source, target, 0);
		if (result.pathWeight < inf_weight) {
			pathTime[i] = result.pathWeight;
		} else {
			pathTime[i] = 0;
		}
		avgPathTime += pathTime[i];
		++i;	
	}

	avgPathTime /= numQueries;
	double variance = 0;
	CRP::weight minVal = avgPathTime;
	CRP::weight maxVal = 0;
	for (CRP::index i = 0; i < numQueries; ++i) {
		variance += ((double) pathTime[i] - avgPathTime) * ((double)pathTime[i] - avgPathTime);
		minVal = std::min(pathTime[i], minVal);
		maxVal = std::max(pathTime[i], maxVal);
	}
	variance /= numQueries;

	std::cout << std::setprecision(3);
	std::cout << "Took " << avgPathTime/1000 << " ms on avgerage. Variance = " << (double) variance / 1000 << " min = " << (double) minVal / 1000 << ", max = " <<(double) maxVal / 1000 << std::endl;

	return 0;
}
