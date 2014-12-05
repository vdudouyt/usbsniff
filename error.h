/*  (c) Valentin Dudouyt, 2013 - 2014
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define ERROR(s) do { fprintf(stderr, "%s\n", (s)); exit(-1); } while(0)
#define ERROR2(...) do { fprintf(stderr, __VA_ARGS__); exit(-1); } while(0)
#define PERROR(s) do { perror((s)); exit(-1); } while(0)
#define USAGE_ERROR(s) do { fprintf(stderr, "%s\n", (s)); print_help_and_exit(argv[0]); } while(0)
#define ASSERT(expr, s) if(!((expr))) ERROR(s)
#define ASSERT2(expr, ...) if(!((expr))) ERROR2(__VA_ARGS__)

#define TRANSFER_FAILED_MESSAGE "Transfer failed: %s\n"
