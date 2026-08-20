# Copilot Instructions

## Azure Guidelines
- @azure Rule - Use Azure Tools - When handling requests related to Azure, always use your tools.
- @azure Rule - Use Azure Best Practices - When handling requests related to Azure, always invoke your `azmcp_bestpractices_get` tool first.
- @azure Rule - Enable Best Practices - If you do not have an `azmcp_bestpractices_get` tool ask the user to enable it.

## Project Structure
- Maintain a professional modular architecture: keep parsers, AST, evaluators, and various mathematical areas in separate .hpp/.cpp files, avoiding large monolithic source files.
- Implement Algebra as a fully independent module containing symbolic expression simplification, linear equations and inequalities, systems of linear equations, polynomials, and functions/graphs. Ensure that graphs are visually engaging, animated, interactive, and designed to make learning intuitive.
